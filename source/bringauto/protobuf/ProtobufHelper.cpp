#include <fleet_protocol/common_headers/general_error_codes.h>

#include <bringauto/protobuf/ProtobufHelper.hpp>

#include <google/protobuf/util/json_util.h>
#include <bringauto/logging/ConsoleSink.hpp>
#include <bringauto/logging/Logger.hpp>

namespace bringauto::protobuf {

using namespace bringauto::logging;
static constexpr LoggerId context = {.id = "id"};
using Logger1 = Logger<context, LoggerImpl>;
//Logger1::addSink<ConsoleSink>();
//Logger1::init(LoggerSettings {"MissionModule", LoggerVerbosity::Debug});

int ProtobufHelper::serializeProtobufMessageToBuffer(struct buffer* message, const google::protobuf::Message &protobufMessage) {
	if (allocate(message, protobufMessage.ByteSizeLong()) == OK) {
		protobufMessage.SerializeToArray(message->data, static_cast<int>(message->size_in_bytes));
		return OK;
	}
	return NOT_OK;
}

int ProtobufHelper::validateAutonomyStatus(const std::string &status) {
	Logger1::addSink<ConsoleSink>();
	Logger1::init(LoggerSettings {"MissionModule", LoggerVerbosity::Debug});
	Logger1::logDebug("status: {}", status);
	MissionModule::AutonomyStatus autonomyStatus {};
	const auto parse_status = google::protobuf::util::JsonStringToMessage(
		status, &autonomyStatus
	);
	Logger1::destroy();
	if(!parse_status.ok()) {
		return NOT_OK;
	}
	return OK;
}

int ProtobufHelper::validateAutonomyCommand(const std::string &command) {
	Logger1::addSink<ConsoleSink>();
	Logger1::init(LoggerSettings {"MissionModule", LoggerVerbosity::Debug});
	Logger1::logDebug("command: {}", command);
	MissionModule::AutonomyCommand autonomyCommand {};
	const auto parse_status = google::protobuf::util::JsonStringToMessage(
		command, &autonomyCommand
	);
	Logger1::destroy();
	if(!parse_status.ok()) {
		return NOT_OK;
	}
	return OK;
}

int ProtobufHelper::validateAutonomyError(const std::string &errorMessage) {
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
