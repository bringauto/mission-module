#include <bringauto/json/DeviceCommand.hpp>
#include <bringauto/json/EnumUtils.hpp>
#include <bringauto/json/SerializationUtils.hpp>
#include <fleet_protocol/common_headers/general_error_codes.h>

#include <nlohmann/json.hpp>

namespace bringauto::json {

using json = nlohmann::ordered_json;

DeviceCommand::DeviceCommand() {

}

int DeviceCommand::serializeToBuffer(buffer *command_buffer) {
    json command_json;
    command_json["route"] = command_.rou;
    command_json["action"] = EnumUtils::autonomyActionToString(command_.act);

    int i = 0;
    for(auto const& stop : command_.sto) {
        command_json["stops"][i]["name"] = stop.nam;
        command_json["stops"][i]["position"]["altitude"] = stop.pos.alt;
        command_json["stops"][i]["position"]["latitude"] = stop.pos.lat;
        command_json["stops"][i]["position"]["longitude"] = stop.pos.lon;
        i++;
    }

    if (SerializationUtils::json_to_buffer(command_json, command_buffer) == NOT_OK) {
        return NOT_OK;
    }

    return OK;
}

int DeviceCommand::deserializeFromBuffer(buffer command_buffer) {
    json command_json;
    char *buffer_data = static_cast<char*> (command_buffer.data);

    try {
        command_json = json::parse(buffer_data, buffer_data + command_buffer.size_in_bytes);
    } catch (json::parse_error &) {
        return NOT_OK;
    }

    try {
        for (int i = 0; i < command_json.at("stops").size(); i++) {
            bringauto::json::Position position {
                .alt = command_json.at("stops").at(i).at("position").at("altitude"),
                .lat = command_json.at("stops").at(i).at("position").at("latitude"),
                .lon = command_json.at("stops").at(i).at("position").at("longitude")
            };
            bringauto::json::Station stop {
                .nam = command_json.at("stops").at(i).at("name"),
                .pos = position
            };
            command_.sto.push_back(stop);
        }

        command_.rou = command_json.at("route");
        command_.act = EnumUtils::stringToAutonomyAction(command_json.at("action"));
    } catch (json::exception &) {
        return NOT_OK;
    }

    return OK;
}

void DeviceCommand::removeFirstStop() {
    if (!command_.sto.empty()) {
        command_.sto.pop_front();
    }
}

}