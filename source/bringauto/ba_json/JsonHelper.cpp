#include <fleet_protocol/common_headers/general_error_codes.h>

#include <bringauto/ba_json/JsonHelper.hpp>


namespace bringauto::ba_json {

using json = nlohmann::ordered_json;

int JsonHelper::bufferToJson(json* json, const buffer& buffer) {
    const auto buffer_data = static_cast<char*> (buffer.data);
    try {
        *json = json::parse(buffer_data, buffer_data + buffer.size_in_bytes);
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

MissionModule::AutonomyStatus_State JsonHelper::stringToAutonomyState(const std::string &state) {
	if (state == "IDLE") {
		return MissionModule::AutonomyStatus_State::AutonomyStatus_State_IDLE;
	} else if (state == "DRIVE") {
		return MissionModule::AutonomyStatus_State::AutonomyStatus_State_DRIVE;
	} else if (state == "IN_STOP") {
		return MissionModule::AutonomyStatus_State::AutonomyStatus_State_IN_STOP;
	} else if (state == "OBSTACLE") {
		return MissionModule::AutonomyStatus_State::AutonomyStatus_State_OBSTACLE;
	} else if (state == "ERROR") {
		return MissionModule::AutonomyStatus_State::AutonomyStatus_State_ERROR;
	}
	return MissionModule::AutonomyStatus_State::AutonomyStatus_State_ERROR;
}

std::string JsonHelper::autonomyStateToString(const MissionModule::AutonomyStatus_State state) {
    switch (state) {
        case MissionModule::AutonomyStatus_State::AutonomyStatus_State_IDLE:
            return "IDLE";
        case MissionModule::AutonomyStatus_State::AutonomyStatus_State_DRIVE:
            return "DRIVE";
        case MissionModule::AutonomyStatus_State::AutonomyStatus_State_IN_STOP:
            return "IN_STOP";
        case MissionModule::AutonomyStatus_State::AutonomyStatus_State_OBSTACLE:
            return "OBSTACLE";
        case MissionModule::AutonomyStatus_State::AutonomyStatus_State_ERROR:
        default:
            return "ERROR";
    }
}

std::string JsonHelper::autonomyActionToString(const MissionModule::AutonomyCommand_Action action) {
    switch (action) {
        case MissionModule::AutonomyCommand_Action::AutonomyCommand_Action_STOP:
            return "STOP";
        case MissionModule::AutonomyCommand_Action::AutonomyCommand_Action_START:
            return "DRIVE";
        case MissionModule::AutonomyCommand_Action::AutonomyCommand_Action_NO_ACTION:
        default:
            return "NO_ACTION";
    }
}

}
