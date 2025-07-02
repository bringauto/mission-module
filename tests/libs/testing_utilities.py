import subprocess
import fleet_http_client_python # type: ignore



PROFILE_ALL = 'all'
PROFILE_MQTT = 'mqtt'

STATE_IDLE = 0
STATE_DRIVE = 1
STATE_IN_STOP = 2


def get_baseline_docker_command(yaml_file: str) -> list[str]:
    return ['docker', 'compose', '--file', yaml_file ]


def run_docker_compose_all(yaml_file: str):
    subprocess.run(get_baseline_docker_command(yaml_file) + ['--profile', PROFILE_ALL, 'up', '-d'], check=True)


def stop_docker_compose_all(yaml_file: str):
    subprocess.run(get_baseline_docker_command(yaml_file) + ['--profile', PROFILE_ALL, 'down'], check=True)


def run_docker_compose_profiles(profiles: list[str], yaml_file: str):
    command = get_baseline_docker_command(yaml_file)
    for profile in profiles:
        command += ['--profile', profile]
    command += ['up', '-d']
    subprocess.run(command, check=True)


def stop_docker_compose_profiles(profiles: list[str], yaml_file: str):
    command = get_baseline_docker_command(yaml_file)
    for profile in profiles:
        command += ['--profile', profile]
    command += ['down']
    subprocess.run(command, check=True)


def kill_docker_component(name: str):
    subprocess.run(['docker', 'stop', name], check=True)


def check_car_state(statuses: list, state: int) -> bool:
    """Check if any of the statuses has the given state."""
    for status in statuses:
        if status.state == state:
            return True
    return False


def check_car_state_and_next_stop(statuses: list, state: int, next_stop: str) -> bool:
    """Check if any of the statuses has the given state and next stop."""
    for status in statuses:
        if status.state == state and status.nextStop.name == next_stop:
            return True
    return False


def get_status_errors() -> list[fleet_http_client_python.Message]:
    """Get all status errors from the fleet HTTP API."""
    configuration = fleet_http_client_python.Configuration(host="http://localhost:8080/v2/protocol",
                                                           api_key={"AdminAuth": "ProtocolStaticAccessKey"})
    api_client = fleet_http_client_python.ApiClient(configuration)
    device_api = fleet_http_client_python.DeviceApi(api_client)
    statuses = device_api.list_statuses(company_name='bringauto',
                                        car_name='virtual_vehicle',
                                        since=0)
    return [status for status in statuses if status.payload.message_type == 'STATUS_ERROR' and status.device_id.module_id == 1]
