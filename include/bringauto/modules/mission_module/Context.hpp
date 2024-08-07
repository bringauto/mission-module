#pragma once

#include <bringauto/fleet_protocol/cxx/DeviceID.hpp>
#include <bringauto/fleet_protocol/http_client/FleetApiClient.hpp>

#include <vector>
#include <condition_variable>

namespace bringauto::modules::mission_module {

struct Context {
    std::shared_ptr<fleet_protocol::http_client::FleetApiClient> fleet_api_client;
    std::vector<device_identification> devices;
    std::vector<std::pair<std::string, fleet_protocol::cxx::DeviceID>> command_vector;
    std::mutex mutex;
    std::condition_variable con_variable;
    long last_command_timestamp;
};

}
