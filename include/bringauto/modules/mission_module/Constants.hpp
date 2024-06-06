#pragma once

#include <chrono>


namespace bringauto::settings {

/**
 * @brief The speed threshold for the status to be sent periodically
 */
constexpr double status_speed_threshold = 1.0;

/**
 * @brief How often status should be sent after the speed threshold is reached
 */
constexpr std::chrono::milliseconds status_sending_period = std::chrono::milliseconds(1000);

}