#include <bringauto/JsonHelper.hpp>

#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>
#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>
#include <bringauto/modules/mission_module/Constants.hpp>

#include <cstring>



namespace bringauto::modules::mission_module::devices {

using json = nlohmann::ordered_json;

std::map<unsigned int, std::chrono::milliseconds> AutonomyDevice::last_sent_status_timestamps_ {};

int AutonomyDevice::send_status_condition(const struct buffer current_status, const struct buffer new_status, unsigned int device_type) {
	json current_status_json {};
	json new_status_json {};
	if (JsonHelper::bufferToJson(current_status_json, current_status) != OK ||
		JsonHelper::bufferToJson(new_status_json, new_status) != OK) {
		return NOT_OK;
	}

	if (current_status_json.at("state") != new_status_json.at("state") ||
		current_status_json.at("nextStop") != new_status_json.at("nextStop")) {
		return OK;
	}

	if (new_status_json.at("telemetry").at("speed") >= Constants::status_speed_threshold) {
		auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		if (last_sent_status_timestamps_[device_type] + Constants::status_sending_period < current_time) {
			last_sent_status_timestamps_[device_type] = current_time;
			return OK;
		}
	}
	return CONDITION_NOT_MET;
}

int AutonomyDevice::generate_command(struct buffer *generated_command, const struct buffer new_status,
									 const struct buffer current_status, const struct buffer current_command) {
	json current_status_json {};
	json new_status_json {};
	json current_command_json {};
	if (JsonHelper::bufferToJson(current_status_json, current_status) != OK ||
		JsonHelper::bufferToJson(new_status_json, new_status) != OK ||
		JsonHelper::bufferToJson(current_command_json, current_command) != OK) {
		return NOT_OK;
	}

	if (!current_command_json.at("stops").empty() &&
		JsonHelper::stringToAutonomyState(std::string(new_status_json.at("state"))) ==
		AutonomyState::IN_STOP &&
		(JsonHelper::stringToAutonomyState(std::string(current_status_json.at("state"))) ==
		AutonomyState::DRIVE ||
		new_status_json.at("nextStop") == current_command_json.at("stops")[0])) {
		current_command_json.at("stops").erase(current_command_json.at("stops").begin());
	}
	return JsonHelper::jsonToBuffer(generated_command, current_command_json);
}

int AutonomyDevice::aggregate_status(struct buffer *aggregated_status, const struct buffer current_status,
									 const struct buffer new_status) {
	if (allocate(aggregated_status, new_status.size_in_bytes) != OK) {
		return NOT_OK;
	}
	std::memcpy(aggregated_status->data, new_status.data, aggregated_status->size_in_bytes);
	return OK;
}

int AutonomyDevice::aggregate_error(struct buffer *error_message, const struct buffer current_error_message,
									const struct buffer status) {
	json status_json {};
	json error_json {};
	if (JsonHelper::bufferToJson(status_json, status) != OK ||
		JsonHelper::bufferToJson(error_json, current_error_message) != OK) {
		return NOT_OK;
	}

	if (status_json.at("state") == JsonHelper::autonomyStateToString(AutonomyState::IN_STOP)) {
		if (!error_json.at("finishedStops").empty()) {
			if (error_json.at("finishedStops")[error_json.at("finishedStops").size() - 1] != status_json.at("nextStop")) {
				error_json.at("finishedStops").push_back(status_json.at("nextStop"));
			}
		} else {
			error_json.at("finishedStops").push_back(status_json.at("nextStop"));
		}
	}

	return JsonHelper::jsonToBuffer(error_message, error_json);
}

int AutonomyDevice::generate_first_command(struct buffer *default_command) {
	json command {};
	command["action"] = JsonHelper::autonomyActionToString(AutonomyAction::NO_ACTION);
	command["route"] = "";
	command["stops"] = json::array();
	if (JsonHelper::jsonToBuffer(default_command, command) != OK) {
		return NOT_OK;
	}
	return OK;
}

int AutonomyDevice::status_data_valid(const struct buffer status) {
	json status_json {};
	if (JsonHelper::bufferToJson(status_json, status) != OK) {
		return NOT_OK;
	}
	return JsonHelper::isValidAutonomyStatus(status_json) ? OK : NOT_OK;
}

int AutonomyDevice::command_data_valid(const struct buffer command) {
	json command_json {};
	if (JsonHelper::bufferToJson(command_json, command) != OK) {
		return NOT_OK;
	}
	return JsonHelper::isValidAutonomyCommand(command_json) ? OK : NOT_OK;
}

}
