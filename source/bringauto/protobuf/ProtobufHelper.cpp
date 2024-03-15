#include <fleet_protocol/common_headers/general_error_codes.h>

#include <bringauto/protobuf/ProtobufHelper.hpp>

namespace bringauto::protobuf {
int ProtobufHelper::serializeProtobufMessageToBuffer(struct buffer* message, const google::protobuf::Message &protobufMessage) {
	if ((allocate(message, protobufMessage.ByteSizeLong())) == OK) {
		protobufMessage.SerializeToArray(message->data, (int)message->size_in_bytes);
		return OK;
	}
	return NOT_OK;
}

MissionModule::AutonomyStatus ProtobufHelper::parseAutonomyStatus(struct buffer status) {
	MissionModule::AutonomyStatus autonomyStatus;
	autonomyStatus.ParseFromArray(status.data, status.size_in_bytes);
	return autonomyStatus;
}

MissionModule::AutonomyCommand ProtobufHelper::parseAutonomyCommand(struct buffer command) {
	MissionModule::AutonomyCommand autonomyCommand;
	autonomyCommand.ParseFromArray(command.data, command.size_in_bytes);
	return autonomyCommand;
}

MissionModule::AutonomyError ProtobufHelper::parseAutonomyError(struct buffer errorMessage) {
	MissionModule::AutonomyError autonomyError;
	autonomyError.ParseFromArray(errorMessage.data, errorMessage.size_in_bytes);
	return autonomyError;
}
}

