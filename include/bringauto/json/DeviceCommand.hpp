#pragma once

#include <bringauto/json/Structures.hpp>
#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::json {

class DeviceCommand {
public:
	DeviceCommand();

	/**
	 * @brief Serializes this object into given buffer using json.
	 */
	int serializeToBuffer(buffer *command_buffer);

	/**
	 * @brief Deserializes command in buffer (json string in data) and sets this object.
	 * @return OK - serialization successful, NOT_OK - serialization failed
	 */
	int deserializeFromBuffer(buffer command_buffer);

	/**
	 * @brief Returns the command of the device.
	 */
	AutonomyCommand getCommand() const { return command_; }

	/**
	 * @brief Sets the action of the device command.
	 */
	void setAction(AutonomyAction action) { command_.act = action; }

	/**
	 * @brief Remove the first top in the stops list.
	 */
	void removeFirstStop();

private:
	AutonomyCommand command_ {};

};

}
