#pragma once

#include <fleet_protocol/common_headers/memory_management.h>

#include <MissionModule.pb.h>

#include <nlohmann/json.hpp>

namespace bringauto::ba_json {

using json = nlohmann::ordered_json;

class JsonHelper {
public:
	static int bufferToJson(json& json, const buffer& buffer);

	static int jsonToBuffer(buffer* buffer, const json& json);

	static MissionModule::AutonomyStatus_State stringToAutonomyState(const std::string_view &state);

	static std::string autonomyStateToString(MissionModule::AutonomyStatus_State state);

	static std::string autonomyActionToString(MissionModule::AutonomyCommand_Action action);

};
}
