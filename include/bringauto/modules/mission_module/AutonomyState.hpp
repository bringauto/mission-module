#pragma once


namespace bringauto::modules::mission_module {

/**
 * @brief Represents the driving state of the autonomy device
 */
enum class AutonomyState {
    IDLE,
    DRIVE,
    IN_STOP,
    OBSTACLE,
    ERROR
};

}