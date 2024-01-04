#pragma once

#include <MissionModule.pb.h>
#include <memory_management.h>

namespace bringauto::modules::mission_module::devices {
class AutonomyDevice {
public:
	/**
	 * @brief Determine whether condition for sending new status was met.
	 *
	 * If current_status is empty, device has just connected.
	 * In this situation it is recommended to return OK, so the status will be sent and the External server will be acknowledged about this device
	 *
	 * @param current_status status data currently present in aggregator in binary form. Can be null for the first call
	 * @param new_status status data sent by client in binary form
	 *
	 * @return OK if condition met.
	 * @return CONDITION_NOT_MET if condition was not met
	 * @return NOT_OK if other error occurred
	*/
	static int send_status_condition(const struct buffer current_status, const struct buffer new_status);

	/**
	 * @brief generate new command based on current status and command and new status
	 *
	 * @param generated_command buffer for storing newly generated command. Look at memory management section
	 * @param new_status newly received status
	 * @param current_status current status present in aggregator. Can be null for the first call
	 * @param current_command current command, that will be updated by the command this function generates.
	 *
	 * @return OK if command generated successfully
	 * @return NOT_OK if other error occurred
	*/
	static int generate_command(struct buffer *generated_command, const struct buffer new_status, const struct buffer current_status, const struct buffer current_command);

	/**
	 * @brief aggregate current status and new status
	 *
	 * @param aggregated_status buffer for storing newly generated status. Look at memory_management section
	 * @param current_status current status present in the aggregator. Can be null
	 * @param new_status newly received status
	 *
	 * @return OK if status aggregated successfully
	 * @return NOT_OK if other error occurred
	*/
	static int aggregate_status(struct buffer *aggregated_status, const struct buffer current_status, const struct buffer new_status);

	/**
	 * @brief aggregate error message
	 *
	 * @param error_message buffer for storing newly generated error message. Look at memory_management section
	 * @param current_error_message current error message present in the aggregator. Can be null
	 * @param status newly received status
	 *
	 * @return OK if error message aggregated successfully
	 * @return NOT_OK if other error occurred
	 */
	static int aggregate_error(struct buffer *error_message, const struct buffer current_error_message, const struct buffer status);

	/**
	 * @brief generate default command
	 * Called when a device is sending first status while adding this status to the aggregator
	 *
	 * @param default_command buffer for storing default command. Look at memory_management section
	 *
	 * @return OK if default command generated successfully
	 * @return NOT_OK if other error occurred
	*/
	static int generate_first_command(struct buffer *default_command);

	/**
	 * @brief Control whether buffer contains valid data
	 *
	 * @param status status data sent by client in binary form
	 * @return OK if status data are valid
	 *         NOT_OK if status data are invalid
	 */
	static int status_data_valid(const struct buffer status);

	/**
	 * @brief Control whether buffer contains valid data
	 *
	 * @param command command data sent by server in binary form
	 * @return OK if command data are valid
	 *         NOT_OK if command data are invalid
	 */
	static int command_data_valid(const struct buffer command);
private:
	static MissionModule::AutonomyCommand generateCommand(std::vector<MissionModule::Station> stops, std::string route,
														  MissionModule::AutonomyCommand::Action action);


};
}