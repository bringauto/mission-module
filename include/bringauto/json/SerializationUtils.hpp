#pragma once

#include <fleet_protocol/common_headers/memory_management.h>

#include <nlohmann/json.hpp>

namespace bringauto::json {

using json = nlohmann::ordered_json;

class SerializationUtils {
public:
	int static json_to_buffer(json json, buffer *buffer);

};

}