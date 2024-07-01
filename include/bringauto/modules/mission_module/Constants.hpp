#pragma once

#include <chrono>


namespace bringauto::modules::mission_module {

/**
 * @brief Constants used in the mission module
 */
class Constants {
public:
    /**
     * @brief The speed threshold for the status to be sent periodically
     */
    inline static constexpr double status_speed_threshold = 1.0;

    /**
     * @brief Minimum time between two status messages being sent when speed is above the threshold
     */
    inline static constexpr std::chrono::milliseconds status_sending_period = std::chrono::milliseconds(2900);
};

}