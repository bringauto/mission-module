#pragma once

#include <MissionModule.pb.h>
#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::protobuf {
class ProtobufHelper {
public:
	static int serializeProtobufMessageToBuffer(struct buffer* message, const google::protobuf::Message &protobufMessage);

	static int validateAutonomyStatus(std::string status);

	static int validateAutonomyCommand(std::string command);

	static int validateAutonomyError(std::string errorMessage);


};
}
