#include <bringauto/protobuf/ProtobufHelper.hpp>
#include <bringauto/ba_json/JsonHelper.hpp>

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
	if (ba_json::JsonHelper::bufferToJson(&current_status_json, current_status) != OK ||
		ba_json::JsonHelper::bufferToJson(&new_status_json, new_status) != OK) {
		return NOT_OK;
	}

	if (current_status_json["state"] != new_status_json["state"] ||
		current_status_json["nextStop"] != new_status_json["nextStop"]) {
		return OK;
	}

	if (new_status_json["telemetry"]["speed"] >= Constants::status_speed_threshold) {
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
	if (ba_json::JsonHelper::bufferToJson(&current_status_json, current_status) != OK ||
		ba_json::JsonHelper::bufferToJson(&new_status_json, new_status) != OK ||
		ba_json::JsonHelper::bufferToJson(&current_command_json, current_command) != OK) {
		return NOT_OK;
	}

	if (ba_json::JsonHelper::stringToAutonomyState(current_status_json["state"]) == MissionModule::AutonomyStatus_State_DRIVE &&
		ba_json::JsonHelper::stringToAutonomyState(new_status_json["state"]) == MissionModule::AutonomyStatus_State_IN_STOP) {
		if (!current_command_json["stops"].empty()) {
			current_command_json["stops"].erase(current_command_json["stops"].begin());
		}
	}
	return ba_json::JsonHelper::jsonToBuffer(generated_command, current_command_json);
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
	if (ba_json::JsonHelper::bufferToJson(&status_json, status) != OK ||
		ba_json::JsonHelper::bufferToJson(&error_json, current_error_message) != OK) {
		return NOT_OK;
	}

	if (status_json["state"] == ba_json::JsonHelper::autonomyStateToString(MissionModule::AutonomyStatus_State_IN_STOP)) {
		if (!error_json["finishedStops"].empty()) {
			if (error_json["finishedStops"][error_json["finishedStops"].size() - 1] != status_json["nextStop"]) {
				error_json["finishedStops"].push_back(status_json["nextStop"]);
			}
		} else {
			error_json["finishedStops"].push_back(status_json["nextStop"]);
		}
	}

	return ba_json::JsonHelper::jsonToBuffer(error_message, error_json);
}

int AutonomyDevice::generate_first_command(struct buffer *default_command) {
	json command {};
	command["action"] = ba_json::JsonHelper::autonomyActionToString(MissionModule::AutonomyCommand_Action_NO_ACTION);
	command["route"] = "";
	command["stops"] = json::array();
	if (ba_json::JsonHelper::jsonToBuffer(default_command, command) != OK) {
		return NOT_OK;
	}
	return OK;
}

int AutonomyDevice::status_data_valid(const struct buffer status) {
	json status_json {};
	if (ba_json::JsonHelper::bufferToJson(&status_json, status) != OK) {
		return NOT_OK;
	}
	if (protobuf::ProtobufHelper::validateAutonomyStatus(nlohmann::to_string(status_json)) != OK) {
		return NOT_OK;
	}
	return OK;
}

int AutonomyDevice::command_data_valid(const struct buffer command) {
	json command_json {};
	if (ba_json::JsonHelper::bufferToJson(&command_json, command) != OK) {
		std::cerr << "Failed to parse command json" << std::endl;
		return NOT_OK;
	}
	std::cerr << command_json.dump() << std::endl;
	if (protobuf::ProtobufHelper::validateAutonomyCommand(nlohmann::to_string(command_json)) != OK) {
		std::cerr << "Failed to validate command" << std::endl;
		return NOT_OK;
	}
	return OK;
}

}