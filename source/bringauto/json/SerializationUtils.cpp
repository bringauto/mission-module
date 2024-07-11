#include <bringauto/json/SerializationUtils.hpp>
#include <fleet_protocol/common_headers/general_error_codes.h>

#include <string>

namespace bringauto::json {

using json = nlohmann::ordered_json;

int SerializationUtils::json_to_buffer(json json, buffer *buffer) {
	std::string tmp = to_string(json);

	if (allocate(buffer, tmp.size()) == NOT_OK) {
		return NOT_OK;
	}

	std::memcpy(buffer->data, tmp.c_str(), tmp.size());

	return OK;
}

}