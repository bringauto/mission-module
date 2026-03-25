#include <bringauto/JsonValidator.hpp>

#include <fleet_protocol/common_headers/general_error_codes.h>
#include <nlohmann/json.hpp>


namespace bringauto {

int JsonValidator::validateAutonomyStatus(const std::string &status) {
    try {
        const auto json = nlohmann::json::parse(status);
        if (!json.contains("state") || !json.contains("telemetry") || !json.contains("nextStop")) {
            return NOT_OK;
        }
    } catch (...) {
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
    } catch (...) {
        return NOT_OK;
    }
    return OK;
}

int JsonValidator::validateAutonomyError(const std::string &errorMessage) {
    try {
        const auto json = nlohmann::json::parse(errorMessage);
        if (!json.contains("finishedStops")) {
            return NOT_OK;
        }
    } catch (...) {
        return NOT_OK;
    }
    return OK;
}

}