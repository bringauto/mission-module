#pragma once

#include <MissionModule.pb.h>
#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::protobuf {
class ProtobufHelper {
public:
	static int serializeProtobufMessageToBuffer(struct buffer* message, const google::protobuf::Message &protobufMessage);

	static int validateAutonomyStatus(const std::string &status);

	static int validateAutonomyCommand(const std::string &command);

	static int validateAutonomyError(const std::string &errorMessage);


};
}
