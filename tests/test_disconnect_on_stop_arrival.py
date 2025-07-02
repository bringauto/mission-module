import argparse
import threading
import time

import libs.MissionModule_pb2
import libs.sniff_mqtt
import libs.testing_utilities as _utils



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Test disconnecting from MQTT broker on stop arrival.')
    parser.add_argument('--yaml', type=str, help='Path to the YAML file for Docker Compose.')
    args = parser.parse_args()

    if not args.yaml:
        print('No YAML file provided. Please use --yaml to specify the path to the Docker Compose YAML file.')
        exit(1)

    print('Starting test: Disconnect on stop arrival')
    _utils.run_docker_compose_all(args.yaml)
    print('Waiting for MQTT broker to start...')
    time.sleep(5)
    mqtt_monitoring = libs.sniff_mqtt.MqttMonitoring(store_statuses=True, store_commands=True, logging=True, verbose=True)
    mqtt_monitoring.connect()
    sniff_thread = threading.Thread(target=mqtt_monitoring.start, daemon=True)
    sniff_thread.start()

    try:
        print('Waiting for car to arrive in stop...')
        while True:
            statuses = mqtt_monitoring.get_mission_module_statuses()
            #print(statuses)
            if _utils.check_car_state_and_next_stop(statuses, _utils.STATE_IN_STOP, 'Svatopluka Čecha A'):
                print('Arrived in stop')

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
                
                mqtt_monitoring.send_mission_module_command(payload=command)

                _utils.kill_docker_component(_utils.NAME_MQTT)
                print('Waiting for 2m 45s for car to arrive at the next stop...')
                time.sleep(165)
                _utils.run_docker_compose_profiles([_utils.PROFILE_MQTT], args.yaml)
                time.sleep(5)
                break

        statuses = _utils.get_status_errors()
        #print(statuses)
        if len(statuses) != 1:
            print('Received unexpected number of status errors:', len(statuses))
        else:
            finished_stops = statuses[0].payload.data.to_dict()['finishedStops']
            if len(finished_stops) != 1:
                print('Unexpected number of finished stops:', len(finished_stops))
            elif finished_stops[0]['name'] != 'Těšínská':
                print('Unexpected finished stop name:', finished_stops[0].name)
            else:
                print('Test passed: Car arrived at the correct stop after disconnecting from the MQTT broker.')


    except KeyboardInterrupt:
        print('KeyboardInterrupt received, stopping...')
    except Exception as e:
        print(f'Error: {e}')

    mqtt_monitoring.stop()
    sniff_thread.join()
    _utils.stop_docker_compose_all(args.yaml)
