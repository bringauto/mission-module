#include <fleet_protocol/common_headers/general_error_codes.h>

#include <bringauto/JsonHelper.hpp>


namespace bringauto {

using json = nlohmann::ordered_json;
using namespace bringauto::modules::mission_module;

int JsonHelper::bufferToJson(json& json, const buffer& buffer) {
    const auto buffer_data = static_cast<char*> (buffer.data);
    try {
        json = json::parse(buffer_data, buffer_data + buffer.size_in_bytes);
    } catch (json::parse_error &) {
        return NOT_OK;
    }
    return OK;
}

int JsonHelper::jsonToBuffer(buffer *buffer, const json& json) {
	const std::string tmp = nlohmann::to_string(json);
	if (allocate(buffer, tmp.size()) == NOT_OK) {
		return NOT_OK;
	}
	std::memcpy(buffer->data, tmp.c_str(), tmp.size());
	return OK;
}

AutonomyState JsonHelper::stringToAutonomyState(const std::string_view state) {
	if (state == "IDLE")     return AutonomyState::IDLE;
	if (state == "DRIVE")    return AutonomyState::DRIVE;
	if (state == "IN_STOP")  return AutonomyState::IN_STOP;
	if (state == "OBSTACLE") return AutonomyState::OBSTACLE;
	return AutonomyState::ERROR;
}

std::string JsonHelper::autonomyStateToString(const AutonomyState state) {
	switch (state) {
		case AutonomyState::IDLE:     return "IDLE";
		case AutonomyState::DRIVE:    return "DRIVE";
		case AutonomyState::IN_STOP:  return "IN_STOP";
		case AutonomyState::OBSTACLE: return "OBSTACLE";
		default:                      return "ERROR";
	}
}

std::string JsonHelper::autonomyActionToString(const AutonomyCommand action) {
	switch (action) {
		case AutonomyCommand::STOP:  return "STOP";
		case AutonomyCommand::START: return "START";
		default:                    return "NO_ACTION";
	}
}

bool JsonHelper::isValidAutonomyStatus(const json& status) {
	return status.contains("state") && status.contains("telemetry") && status.contains("nextStop");
}

bool JsonHelper::isValidAutonomyCommand(const json& command) {
	return command.contains("action") && command.contains("stops") && command.contains("route");
}

bool JsonHelper::isValidAutonomyError(const json& errorMessage) {
	return errorMessage.contains("finishedStops");
}

}