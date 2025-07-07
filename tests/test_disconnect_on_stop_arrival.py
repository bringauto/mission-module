import argparse
import threading
import time

import libs.MissionModule_pb2
import libs.sniff_mqtt
import libs.testing_utilities as _utils



def create_command() -> libs.MissionModule_pb2.AutonomyCommand:
    """Creates a command that is expected in the default etna scenario since it reaches the first stop."""
    command = libs.MissionModule_pb2.AutonomyCommand()
    command.action = libs.MissionModule_pb2.AutonomyCommand.Action.START
    command.route = 'Moravské náměstí 2'
    
    stops: list[libs.MissionModule_pb2.Station()] = []
    stops.append(libs.MissionModule_pb2.Station(name='Svatopluka Čecha A',
                                                position=libs.MissionModule_pb2.Position(latitude=49.221645,
                                                                                            longitude=16.59081)))
    stops.append(libs.MissionModule_pb2.Station(name='Těšínská',
                                                position=libs.MissionModule_pb2.Position(latitude=49.22316,
                                                                                            longitude=16.58995)))
    command.stops.extend(stops)
    return command


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Test disconnecting from MQTT broker on stop arrival.')
    parser.add_argument('--yaml', type=str, help='Path to the YAML file for Docker Compose.')
    parser.add_argument('--logging', action='store_true', help='Enable mqtt sniffer logging for the test run.')
    parser.add_argument('--verbose', action='store_true', help='Enable verbose output for mqtt sniffer logs.')
    args = parser.parse_args()

    if not args.yaml:
        print('No YAML file provided. Please use --yaml to specify the path to the Docker Compose YAML file.')
        exit(1)

    test_passed = False

    # Construct the vernemq docker component name based on the YAML file path.
    mqtt_component_name = str(args.yaml).split('/')[-2] + '-vernemq-1'

    print('Starting test: Disconnect on stop arrival')
    _utils.run_docker_compose_all(args.yaml)
    print('Waiting for MQTT broker to start...')
    time.sleep(5)
    mqtt_monitoring = libs.sniff_mqtt.MqttMonitoring(store_statuses=True,
                                                     store_commands=True,
                                                     logging=args.logging,
                                                     verbose=args.verbose)
    mqtt_monitoring.connect()
    sniff_thread = threading.Thread(target=mqtt_monitoring.start, daemon=True)
    sniff_thread.start()

    try:
        print('Waiting for car to arrive in stop...')
        while True:
            statuses = mqtt_monitoring.get_mission_module_statuses()
            #print(statuses)
            # Wait until the car reaches the Svatopluka Čecha A stop.
            if _utils.check_car_state_and_next_stop(statuses, _utils.STATE_IN_STOP, 'Svatopluka Čecha A'):
                print('Arrived in stop')
                # Send a command that would normally cause the car to be stuck in the stop. Disconnect the MQTT broker at the same time.
                mqtt_monitoring.send_mission_module_command(payload=create_command())
                _utils.stop_docker_component(mqtt_component_name)
                
                print('Waiting for 2m 45s for car to arrive at the next stop...')
                time.sleep(165)
                _utils.run_docker_compose_profiles([_utils.PROFILE_MQTT], args.yaml)
                
                print('Waiting for 30 seconds to ensure the car has time to process the reconnection...')
                time.sleep(30)
                break

        # Check if upon reconnection the car already arrived at the Těšínská stop.
        statuses = _utils.get_status_errors()
        #print(statuses)
        if len(statuses) != 1:
            print('Received unexpected number of status errors:', len(statuses))
        else:
            finished_stops = statuses[0].payload.data.to_dict()['finishedStops']
            if len(finished_stops) != 1:
                print('Unexpected number of finished stops:', len(finished_stops))
            elif finished_stops[0]['name'] != 'Těšínská':
                print('Unexpected finished stop name:', finished_stops[0]['name'])
            else:
                print('Car arrived at the correct stop after disconnecting from the MQTT broker.')
                test_passed = True


    except KeyboardInterrupt:
        print('KeyboardInterrupt received, stopping...')
    except Exception as e:
        print(f'Error: {e}')

    mqtt_monitoring.stop()
    sniff_thread.join()
    _utils.stop_docker_compose_all(args.yaml)
    
    if test_passed:
        print('\n\033[92mTest passed.\033[0m')
    else:
        print('\n\033[31mTest failed.\033[0m')
        exit(1)
