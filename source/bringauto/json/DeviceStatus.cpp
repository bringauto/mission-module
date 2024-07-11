#include <bringauto/json/DeviceStatus.hpp>
#include <bringauto/json/EnumUtils.hpp>
#include <bringauto/json/SerializationUtils.hpp>
#include <fleet_protocol/common_headers/general_error_codes.h>

#include <nlohmann/json.hpp>

namespace bringauto::json {

using json = nlohmann::ordered_json;

DeviceStatus::DeviceStatus() {

}

int DeviceStatus::serializeToBuffer(buffer *status_buffer) {
    json status_json;
    status_json["telemetry"]["fuel"] = status_.tel.fue;
    status_json["telemetry"]["speed"] = status_.tel.spe;
    status_json["telemetry"]["position"]["altitude"] = status_.tel.pos.alt;
    status_json["telemetry"]["position"]["latitude"] = status_.tel.pos.lat;
    status_json["telemetry"]["position"]["longitude"] = status_.tel.pos.lon;
    status_json["nextStop"]["name"] = status_.nex.nam;
    status_json["nextStop"]["position"]["altitude"] = status_.nex.pos.alt;
    status_json["nextStop"]["position"]["latitude"] = status_.nex.pos.lat;
    status_json["nextStop"]["position"]["longitude"] = status_.nex.pos.lon;
    status_json["state"] = EnumUtils::autonomyStateToString(status_.sta);

    if (SerializationUtils::json_to_buffer(status_json, status_buffer) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int DeviceStatus::deserializeFromBuffer(buffer status_buffer) {
    json status_json;
    char *buffer_data = static_cast<char*> (status_buffer.data);

    try {
        status_json = json::parse(buffer_data, buffer_data + status_buffer.size_in_bytes);
    } catch (json::parse_error &) {
        return NOT_OK;
    }

    try {
        bringauto::json::Position position {
            .alt = status_json.at("telemetry").at("position").at("altitude"),
            .lat = status_json.at("telemetry").at("position").at("latitude"),
            .lon = status_json.at("telemetry").at("position").at("longitude")
        };
        bringauto::json::Telemetry telemetry {
            .spe = status_json.at("telemetry").at("speed"),
            .fue = status_json.at("telemetry").at("fuel"),
            .pos = position
        };
        bringauto::json::Position next_stop_position {
            .alt = status_json.at("nextStop").at("position").at("altitude"),
            .lat = status_json.at("nextStop").at("position").at("latitude"),
            .lon = status_json.at("nextStop").at("position").at("longitude")
        };
        bringauto::json::Station next_stop {
            .nam = status_json.at("nextStop").at("name"),
            .pos = next_stop_position
        };
        status_.nex = next_stop;
        status_.tel = telemetry;
        status_.sta = EnumUtils::stringToAutonomyState(status_json.at("state"));
    } catch (json::exception &) {
        return NOT_OK;
    }

    return OK;
}

}