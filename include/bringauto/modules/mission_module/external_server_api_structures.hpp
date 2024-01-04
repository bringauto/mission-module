#pragma once

#include <bringauto/fleet_protocol/cxx/DeviceID.hpp>

#include <vector>
#include <condition_variable>
#include <thread>

namespace bringauto::modules::mission_module {

struct context {
    std::vector<device_identification> devices;
    std::vector<std::pair<std::string, bringauto::fleet_protocol::cxx::DeviceID>> command_vector;
    std::mutex mutex;
    std::condition_variable con_variable;
    std::string api_url;
    std::string api_key;
    std::string company_name;
    std::string car_name;
    long last_command_timestamp;
};

}
