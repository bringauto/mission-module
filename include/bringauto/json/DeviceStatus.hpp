#pragma once

#include <bringauto/json/Structures.hpp>
#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::json {

class DeviceStatus {
public:
	DeviceStatus();	

	/**
	 * @brief Serializes this object into given buffer using json.
	 */
	int serializeToBuffer(buffer *status_buffer);

	/**
	 * @brief Deserializes status in buffer (json string in data) and sets this object.
	 * @return OK - serialization successful, NOT_OK - serialization failed
	 */
	int deserializeFromBuffer(buffer status_buffer);

	/**
	 * @brief Returns the status of the device.
	 */
	AutonomyStatus getStatus() const { return status_; }

private:
	AutonomyStatus status_ {};

};

}
