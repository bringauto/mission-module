#pragma once

#include <chrono>


namespace bringauto::modules::mission_module {

/**
 * @brief The speed threshold for the status to be sent periodically
 */
constexpr double status_speed_threshold = 1.0;

/**
 * @brief Minimum time between two status messages being sent when speed is above the threshold
 */
constexpr std::chrono::milliseconds status_sending_period = std::chrono::milliseconds(2900);

}