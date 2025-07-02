import time, threading

import libs.ExternalProtocol_pb2
import libs.InternalProtocol_pb2
import libs.MissionModule_pb2

from paho.mqtt.client import (
    Client, MQTTMessage, MQTTv311
)
from paho.mqtt.enums import CallbackAPIVersion



MISSION_MODULE_NUMBER = 1
MAX_MESSAGES = 100


def mission_module_device() -> libs.InternalProtocol_pb2.Device:
    device = libs.InternalProtocol_pb2.Device()
    device.module = MISSION_MODULE_NUMBER
    device.deviceType = 1
    device.deviceRole = 'autonomy'
    device.deviceName = 'Autonomy'
    return device


class MqttMonitoring:
    def __init__(self, verbose: bool = False, store_statuses: bool = False, store_commands: bool = False, logging: bool = False):
        self._create_client()
        self._verbose = verbose
        self._logging = logging
        self._store_statuses = store_statuses
        self._store_commands = store_commands
        self._statuses = []
        self._commands = []
        self._status_lock = threading.Lock()
        self._status_condition = threading.Condition(self._status_lock)
        self._command_lock = threading.Lock()
        self._command_condition = threading.Condition(self._command_lock)
        self._mission_module_command_counter = 0
        self._mission_module_session_id = ''


    def _create_client(self):
        self._client =Client(
            client_id='Monitoring Test',
            protocol=MQTTv311,
            transport='tcp',
            callback_api_version=CallbackAPIVersion.VERSION2,
            reconnect_on_failure=True,
        )
        self._client.on_connect = self._on_connect
        self._client.on_message = self._on_message
                

    def connect(self, host: str = '127.0.0.1', port: int = 1883, keepalive: int = 5) -> None:
        """Connects to the MQTT broker with retry logic."""
        connected = False
        while not connected:
            try:
                self._create_client()
                self._client.connect(host=host, port=port, keepalive=keepalive)
                connected = True
            except ConnectionRefusedError:
                time.sleep(2)


    def start(self) -> None:
        """Starts the MQTT client loop to listen for messages."""
        self._client.loop_forever(timeout=60)


    def stop(self) -> None:
        """Stops the MQTT client loop and disconnects from the broker."""
        self._client.loop_stop()
        self._client.disconnect()


    def _on_connect(self, client, userdata, flags, rc, properties) -> None:
        self._client.subscribe("#", qos=1)


    def _on_message(self, client, userdata, message: MQTTMessage) -> None:
        if self._verbose: self._print(f'topic: {message.topic}')
        message_type = message.topic.split('/')[-1]
        if message_type == 'module_gateway':
            self._print('\nMessage from module gateway:')
            self._decode_module_gateway_message(message.payload)
        elif message_type == 'external_server':
            self._print('\nMessage from external server:')
            self._decode_external_server_message(message.payload)
        else:
            self._print(f'\nUnknown message type: {message_type}')


    def _decode_module_gateway_message(self, payload: bytes):
        message = libs.ExternalProtocol_pb2.ExternalClient()
        message.ParseFromString(payload)
        if message.HasField('status'):
            self._handle_status(getattr(message, 'status').deviceStatus)
        elif message.HasField('connect'):
            self._print('\tConnect message sent')
        elif message.HasField('commandResponse'):
            self._print('\tCommand response sent')
        else:
            self._print(f'\tMessage with unhandled type')
        if self._verbose: self._print(message)


    def _decode_external_server_message(self, payload: bytes):
        message = libs.ExternalProtocol_pb2.ExternalServer()
        message.ParseFromString(payload)
        if message.HasField('command'):
            self._handle_command(getattr(message, 'command'))
        elif message.HasField('statusResponse'):
            self._print('\tStatus response sent')
        elif message.HasField('connectResponse'):
            self._print('\tConnect response sent')
        else:
            self._print(f'\tMessage with unhandled type')
        if self._verbose: self._print(message)


    def _handle_status(self, status):
        if status.device.module == MISSION_MODULE_NUMBER:
            self._handle_mission_module_status(status.statusData)
        else:
            self._print(f'\tStatus from unsupported module: {status.device.module}')


    def _handle_mission_module_status(self, status):
        mission_status = libs.MissionModule_pb2.AutonomyStatus()
        mission_status.ParseFromString(status)
        if self._store_statuses:
            with self._status_condition:
                self._statuses.append(mission_status)
                if len(self._statuses) > MAX_MESSAGES:
                    self._statuses.pop(0)
                self._status_condition.notify_all()
        self._print(f'\tMission Module Status:\n--------------------\n{mission_status}\n--------------------')


    def get_mission_module_statuses(self):
        """Returns a copy of the stored mission module statuses and clears the storage."""
        with self._status_condition:
            while not self._statuses:
                self._status_condition.wait()
            ret = self._statuses.copy()
            self._statuses.clear()
            return ret


    def _handle_command(self, command):
        if command.deviceCommand.device.module == MISSION_MODULE_NUMBER:
            self._mission_module_command_counter = command.messageCounter
            self._mission_module_session_id = command.sessionId
            self._handle_mission_module_command(command.deviceCommand.commandData)
        else:
            self._print(f'\tCommand for unsupported module: {command.deviceCommand.device.module}')


    def _handle_mission_module_command(self, command):
        mission_command = libs.MissionModule_pb2.AutonomyCommand()
        mission_command.ParseFromString(command)
        if self._store_commands:
            with self._command_condition:
                self._commands.append(mission_command)
                if len(self._commands) > MAX_MESSAGES:
                    self._commands.pop(0)
                self._command_condition.notify_all()
        self._print(f'\tMission Module Command:\n--------------------\n{mission_command}\n--------------------')


    def get_mission_module_commands(self):
        """Returns a copy of the stored mission module commands and clears the storage."""
        with self._command_condition:
            while not self._commands:
                self._command_condition.wait()
            ret = self._commands.copy()
            self._commands.clear()
            return ret
        
    
    def _print(self, message: str):
        """Prints a message if logging is enabled."""
        if self._logging:
            print(message)


    def send_mission_module_command(self, payload: libs.MissionModule_pb2.AutonomyCommand) -> None:
        """Sends a mission module command to the MQTT broker."""
        if not isinstance(payload, libs.MissionModule_pb2.AutonomyCommand):
            raise TypeError('Command must be of type AutonomyCommand')
        
        command = libs.ExternalProtocol_pb2.Command()
        command.sessionId = self._mission_module_session_id
        command.messageCounter = self._mission_module_command_counter + 1

        device_command = libs.InternalProtocol_pb2.DeviceCommand()
        device_command.device.CopyFrom(mission_module_device())
        device_command.commandData = payload.SerializeToString()
        command.deviceCommand.CopyFrom(device_command)

        msg = libs.ExternalProtocol_pb2.ExternalServer()
        msg.command.CopyFrom(command)

        self._client.publish(
            topic=f'bringauto/virtual_vehicle/external_server',
            payload=msg.SerializeToString(),
            qos=1
        )



if __name__ == '__main__':
    print('Waiting for MQTT broker connection...')
    mqtt_monitoring = MqttMonitoring(logging=True)
    mqtt_monitoring.connect()
    mqtt_monitoring.start()
