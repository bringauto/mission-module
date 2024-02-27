#pragma once

#include <MissionModule.pb.h>
#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::protobuf {
class ProtobufHelper {
public:
	static int serializeProtobufMessageToBuffer(struct buffer* message, const google::protobuf::Message &protobufMessage);

	static MissionModule::AutonomyStatus parseAutonomyStatus(struct buffer status);

	static MissionModule::AutonomyCommand parseAutonomyCommand(struct buffer command);

	static MissionModule::AutonomyError parseAutonomyError(struct buffer errorMessage);


};
}