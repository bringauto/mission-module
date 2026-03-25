#pragma once

#include <string>


namespace bringauto {

/**
 * @brief Validates JSON messages against the expected mission module message schemas
 */
class JsonValidator {
public:
    /**
     * @brief Validates that a JSON string contains the required fields for an AutonomyStatus message
     * @param status JSON string to validate
     * @return OK if valid, NOT_OK otherwise
     */
    static int validateAutonomyStatus(const std::string &status);

    /**
     * @brief Validates that a JSON string contains the required fields for an AutonomyCommand message
     * @param command JSON string to validate
     * @return OK if valid, NOT_OK otherwise
     */
    static int validateAutonomyCommand(const std::string &command);

    /**
     * @brief Validates that a JSON string contains the required fields for an AutonomyError message
     * @param errorMessage JSON string to validate
     * @return OK if valid, NOT_OK otherwise
     */
    static int validateAutonomyError(const std::string &errorMessage);
};

}