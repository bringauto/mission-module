#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>
#include <device_management.h>

#include <bringauto/modules/mission_module/MissionModule.hpp>
/// Devices
#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>
/// Devices

namespace bamm = bringauto::modules::mission_module;

[[nodiscard]] int send_status_condition(const struct buffer current_status, const struct buffer new_status, unsigned int device_type) {
	switch (device_type) {
		case bamm::MISSION_MODULE_NUMBER:
			return bamm::devices::AutonomyDevice::send_status_condition(current_status, new_status);
		default:
			return NOT_OK;
	}
}

[[nodiscard]] int
generate_command(struct buffer *generated_command, const struct buffer new_status, const struct buffer current_status,
				 const struct buffer current_command, unsigned int device_type) {
	switch (device_type) {
		case bamm::MISSION_MODULE_NUMBER:
			return bamm::devices::AutonomyDevice::generate_command(generated_command, new_status, current_status, current_command);
		default:
			return NOT_OK;
	}
}

[[nodiscard]] int
aggregate_status(struct buffer *aggregated_status, const struct buffer current_status, const struct buffer new_status,
				 unsigned int device_type) {
	switch (device_type) {
		case bamm::MISSION_MODULE_NUMBER:
			return bamm::devices::AutonomyDevice::aggregate_status(aggregated_status, current_status, new_status);
		default:
			return NOT_OK;
	}
}

[[nodiscard]] int aggregate_error(struct buffer *error_message, const struct buffer current_error_message, const struct buffer status,
					unsigned int device_type) {
	switch (device_type) {
		case bamm::MISSION_MODULE_NUMBER:
			return bamm::devices::AutonomyDevice::aggregate_error(error_message, current_error_message, status);
		default:
			return NOT_OK;
		}
}

[[nodiscard]] int generate_first_command(struct buffer *default_command, unsigned int device_type) {
	switch (device_type) {
		case bamm::MISSION_MODULE_NUMBER:
			return bamm::devices::AutonomyDevice::generate_first_command(default_command);
		default:
			return NOT_OK;
	}
}

[[nodiscard]] int status_data_valid(const struct buffer status, unsigned int device_type) {
	switch (device_type) {
		case bamm::AUTONOMY_DEVICE_TYPE:
			return bamm::devices::AutonomyDevice::status_data_valid(status);
		default:
			return NOT_OK;
	}
}

[[nodiscard]] int command_data_valid(const struct buffer command, unsigned int device_type) {
	switch (device_type) {
		case bamm::AUTONOMY_DEVICE_TYPE:
			return bamm::devices::AutonomyDevice::status_data_valid(command);
		default:
			return NOT_OK;
	}
}
