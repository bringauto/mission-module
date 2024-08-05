#include <fleet_protocol/common_headers/general_error_codes.h>

#include <bringauto/protobuf/ProtobufHelper.hpp>

#include <google/protobuf/util/json_util.h>

namespace bringauto::protobuf {
int ProtobufHelper::serializeProtobufMessageToBuffer(struct buffer* message, const google::protobuf::Message &protobufMessage) {
	if ((allocate(message, protobufMessage.ByteSizeLong())) == OK) {
		protobufMessage.SerializeToArray(message->data, (int)message->size_in_bytes);
		return OK;
	}
	return NOT_OK;
}

int ProtobufHelper::validateAutonomyStatus(std::string status) {
	MissionModule::AutonomyStatus autonomyStatus {};
	const auto parse_status = google::protobuf::util::JsonStringToMessage(
		status, &autonomyStatus
	);
	if(!parse_status.ok()) {
		return NOT_OK;
	}
	return OK;
}

int ProtobufHelper::validateAutonomyCommand(std::string command) {
	MissionModule::AutonomyCommand autonomyCommand {};
	const auto parse_status = google::protobuf::util::JsonStringToMessage(
		command, &autonomyCommand
	);
	if(!parse_status.ok()) {
		return NOT_OK;
	}
	return OK;
}

int ProtobufHelper::validateAutonomyError(std::string errorMessage) {
	MissionModule::AutonomyError autonomyError {};
	const auto parse_status = google::protobuf::util::JsonStringToMessage(
		errorMessage, &autonomyError
	);
	if(!parse_status.ok()) {
		return NOT_OK;
	}
	return OK;
}
}
