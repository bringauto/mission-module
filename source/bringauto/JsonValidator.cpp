#include <bringauto/JsonValidator.hpp>
#include <bringauto/JsonHelper.hpp>

#include <fleet_protocol/common_headers/general_error_codes.h>
#include <nlohmann/json.hpp>


namespace bringauto {

int JsonValidator::validateAutonomyStatus(const std::string &status) {
    try {
        const auto json = nlohmann::json::parse(status);
        if (!json.contains("state") || !json.contains("telemetry") || !json.contains("nextStop")) {
            return NOT_OK;
        }
        if (!json.at("state").is_string()) {
            return NOT_OK;
        }
        if (!JsonHelper::isValidAutonomyStateString(json.at("state").get<std::string>())) {
            return NOT_OK;
        }
        if (!json.at("telemetry").is_object() || !json.at("telemetry").contains("speed")) {
            return NOT_OK;
        }
    } catch (const nlohmann::json::exception &) {
        return NOT_OK;
    }
    return OK;
}

int JsonValidator::validateAutonomyCommand(const std::string &command) {
    try {
        const auto json = nlohmann::json::parse(command);
        if (!json.contains("action") || !json.contains("stops") || !json.contains("route")) {
            return NOT_OK;
        }
        if (!json.at("action").is_string()) {
            return NOT_OK;
        }
        if (!JsonHelper::isValidAutonomyCommandString(json.at("action").get<std::string>())) {
            return NOT_OK;
        }
        if (!json.at("stops").is_array()) {
            return NOT_OK;
        }
        if (!json.at("route").is_string()) {
            return NOT_OK;
        }
    } catch (const nlohmann::json::exception &) {
        return NOT_OK;
    }
    return OK;
}

int JsonValidator::validateAutonomyError(const std::string &errorMessage) {
    try {
        const auto json = nlohmann::json::parse(errorMessage);
        if (!json.contains("finishedStops") || !json.at("finishedStops").is_array()) {
            return NOT_OK;
        }
    } catch (const nlohmann::json::exception &) {
        return NOT_OK;
    }
    return OK;
}

}