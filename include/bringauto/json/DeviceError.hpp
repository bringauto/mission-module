#pragma once

#include <bringauto/json/Structures.hpp>
#include <fleet_protocol/common_headers/memory_management.h>

namespace bringauto::json {

class DeviceError {
public:
	DeviceError();	

	/**
	 * @brief Serializes this object into given buffer using json.
	 */
	int serializeToBuffer(buffer *error_buffer);

	/**
	 * @brief Deserializes error in buffer (json string in data) and sets this object.
	 * @return OK - serialization successful, NOT_OK - serialization failed
	 */
	int deserializeFromBuffer(buffer error_buffer);

	/**
	 * @brief Returns the error of the device.
	 */
	AutonomyError getError() const { return error_; }

    /**
     * @brief Add a stop to the finished stops list.
     */
    void addFinishedStop(const Station &station);

private:
	AutonomyError error_ {};

};

}
