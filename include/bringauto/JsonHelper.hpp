#pragma once

#include <bringauto/modules/mission_module/AutonomyCommand.hpp>
#include <bringauto/modules/mission_module/AutonomyState.hpp>
#include <fleet_protocol/common_headers/memory_management.h>

#include <nlohmann/json.hpp>


namespace bringauto {

/**
 * @brief Utility class for converting between buffers, JSON objects, and mission module enums
 *
 * Represents a collection of static conversion and validation utilities tied to the
 * mission module's JSON message format. Its purpose is to centralise all JSON serialisation,
 * deserialisation, and structural validation so that the rest of the module does not
 * depend directly on the nlohmann::json API.
 */
class JsonHelper {
public:
    /**
     * @brief Parses a buffer into a JSON object
     * @param json Output JSON object
     * @param buffer Input buffer containing a JSON string
     * @return OK on success, NOT_OK if parsing fails
     */
    static int bufferToJson(nlohmann::ordered_json& json, const buffer& buffer);

    /**
     * @brief Serializes a JSON object into a buffer
     * @param buffer Output buffer
     * @param json Input JSON object
     * @return OK on success, NOT_OK if allocation fails
     */
    static int jsonToBuffer(buffer* buffer, const nlohmann::ordered_json& json);

    /**
     * @brief Converts a string representation of autonomy state to the corresponding enum value
     * @param state String state value (e.g. "DRIVE", "IN_STOP")
     * @return Corresponding AutonomyState enum value, ERROR if unrecognized
     */
    static modules::mission_module::AutonomyState stringToAutonomyState(std::string_view state);

    /**
     * @brief Converts an AutonomyState enum value to its string representation
     * @param state AutonomyState enum value
     * @return String representation (e.g. "DRIVE", "IN_STOP")
     */
    static std::string autonomyStateToString(modules::mission_module::AutonomyState state);

    /**
     * @brief Converts an AutonomyCommand enum value to its string representation
     * @param action AutonomyCommand enum value
     * @return String representation (e.g. "START", "STOP", "NO_ACTION")
     */
    static std::string autonomyActionToString(modules::mission_module::AutonomyCommand action);

    /**
     * @brief Checks whether a JSON object contains all required fields for an AutonomyStatus message
     * @param status JSON object to validate
     * @return true if all required fields are present
     */
    static bool isValidAutonomyStatus(const nlohmann::ordered_json& status);

    /**
     * @brief Checks whether a JSON object contains all required fields for an AutonomyCommand message
     * @param command JSON object to validate
     * @return true if all required fields are present
     */
    static bool isValidAutonomyCommand(const nlohmann::ordered_json& command);

    /**
     * @brief Checks whether a JSON object contains all required fields for an AutonomyError message
     * @param errorMessage JSON object to validate
     * @return true if all required fields are present
     */
    static bool isValidAutonomyError(const nlohmann::ordered_json& errorMessage);
};

}
