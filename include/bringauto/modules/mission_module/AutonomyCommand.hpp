#pragma once


namespace bringauto::modules::mission_module {

/**
 * @brief Represents the action to be performed by the autonomy device
 */
enum class AutonomyCommand {
    NO_ACTION,
    STOP,
    START
};

}