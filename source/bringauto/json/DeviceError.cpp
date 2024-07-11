#include <bringauto/json/DeviceError.hpp>
#include <bringauto/json/EnumUtils.hpp>
#include <bringauto/json/SerializationUtils.hpp>
#include <fleet_protocol/common_headers/general_error_codes.h>

#include <nlohmann/json.hpp>

namespace bringauto::json {

using json = nlohmann::ordered_json;

DeviceError::DeviceError() {

}

int DeviceError::serializeToBuffer(buffer *error_buffer) {
    json error_json;

    int i = 0;
    for(auto const& stop : error_.fin) {
        error_json["finishedStops"][i]["name"] = stop.nam;
        error_json["finishedStops"][i]["position"]["altitude"] = stop.pos.alt;
        error_json["finishedStops"][i]["position"]["latitude"] = stop.pos.lat;
        error_json["finishedStops"][i]["position"]["longitude"] = stop.pos.lon;
        i++;
    }

    if (SerializationUtils::json_to_buffer(error_json, error_buffer) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int DeviceError::deserializeFromBuffer(buffer error_buffer) {
    json error_json;
    char *buffer_data = static_cast<char*> (error_buffer.data);

    try {
        error_json = json::parse(buffer_data, buffer_data + error_buffer.size_in_bytes);
    } catch (json::parse_error &) {
        return NOT_OK;
    }

    try {
        for (int i = 0; i < error_json.at("finishedStops").size(); i++) {
            bringauto::json::Position position {
                .alt = error_json.at("finishedStops").at(i).at("position").at("altitude"),
                .lat = error_json.at("finishedStops").at(i).at("position").at("latitude"),
                .lon = error_json.at("finishedStops").at(i).at("position").at("longitude")
            };
            bringauto::json::Station stop {
                .nam = error_json.at("finishedStops").at(i).at("name"),
                .pos = position
            };
            error_.fin.push_back(stop);
        }
    } catch (json::exception &) {
        return NOT_OK;
    }

    return OK;
}

void DeviceError::addFinishedStop(const Station &station) {
    error_.fin.push_back(station);
}

}
