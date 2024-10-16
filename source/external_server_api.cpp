#include <fleet_protocol/module_maintainer/external_server/external_server_interface.h>
#include <bringauto/modules/mission_module/Context.hpp>
#include <bringauto/modules/mission_module/MissionModule.hpp>
#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>
#include <bringauto/protobuf/ProtobufHelper.hpp>
#include <bringauto/fleet_protocol/cxx/DeviceID.hpp>
#include <bringauto/fleet_protocol/cxx/KeyValueConfig.hpp>
#include <bringauto/fleet_protocol/cxx/StringAsBuffer.hpp>
#include <bringauto/fleet_protocol/http_client/FleetApiClient.hpp>
 
#include <vector>
#include <cstring>
#include <regex>


namespace bamm = bringauto::modules::mission_module;

void *init(const config config_data) {
    auto *context = new bamm::Context {};
    const bringauto::fleet_protocol::cxx::KeyValueConfig config(config_data);
    std::string api_url {};
    std::string api_key {};
    std::string company_name {};
    std::string car_name {};
    int max_requests_threshold_count {};
    int max_requests_threshold_period_ms {};
    int delay_after_threshold_reached_ms {};
    int retry_requests_delay_ms {};

    for (auto i = config.cbegin(); i != config.cend(); ++i) {
        if (i->first == "api_url") {
            if (!std::regex_match(i->second, std::regex(R"(^(http|https)://([\w-]+\.)?+[\w-]+(:[0-9]+)?(/[\w-]*)?+$)"))) {
                delete context;
                return nullptr;
            }
            api_url = i->second;
        }
        else if (i->first == "api_key") {
            if (i->second.empty()) {
                delete context;
                return nullptr;
            }
            api_key = i->second;
        }
        else if (i->first == "company_name") {
            if (!std::regex_match(i->second, std::regex("^[a-z0-9_]*$")) || i->second.empty()) {
                delete context;
                return nullptr;
            }
            company_name = i->second;
        }
        else if (i->first == "car_name") {
            if (!std::regex_match(i->second, std::regex("^[a-z0-9_]*$")) || i->second.empty()) {
                delete context;
                return nullptr;
            }
            car_name = i->second;
        }
        else if (i->first == "max_requests_threshold_count") {
            auto result = std::from_chars(i->second.data(), i->second.data() + i->second.size(), max_requests_threshold_count);
            if (result.ec == std::errc() || max_requests_threshold_count < 0 || i->second.empty()) {
                delete context;
                return nullptr;
            }
        }
        else if (i->first == "max_requests_threshold_period_ms") {
            auto result = std::from_chars(i->second.data(), i->second.data() + i->second.size(), max_requests_threshold_period_ms);
            if (result.ec == std::errc() || max_requests_threshold_period_ms < 0 || i->second.empty()) {
                delete context;
                return nullptr;
            }
        }
        else if (i->first == "delay_after_threshold_reached_ms") {
            auto result = std::from_chars(i->second.data(), i->second.data() + i->second.size(), delay_after_threshold_reached_ms);
            if (result.ec == std::errc() || delay_after_threshold_reached_ms < 0 || i->second.empty()) {
                delete context;
                return nullptr;
            }
        }
        else if (i->first == "retry_requests_delay_ms") {
            auto result = std::from_chars(i->second.data(), i->second.data() + i->second.size(), retry_requests_delay_ms);
            if (result.ec == std::errc() || retry_requests_delay_ms < 0 || i->second.empty()) {
                delete context;
                return nullptr;
            }
        }
    }

    bringauto::fleet_protocol::http_client::FleetApiClient::FleetApiClientConfig fleet_api_config {
        .apiUrl = api_url,
        .apiKey = api_key,
        .companyName = company_name,
        .carName = car_name
    };

    bringauto::fleet_protocol::http_client::RequestFrequencyGuard::RequestFrequencyGuardConfig request_frequency_guard_config {
        .maxRequestsThresholdCount = max_requests_threshold_count,
        .maxRequestsThresholdPeriodMs = std::chrono::milliseconds(max_requests_threshold_period_ms),
        .delayAfterThresholdReachedMs = std::chrono::milliseconds(delay_after_threshold_reached_ms),
        .retryRequestsDelayMs = std::chrono::milliseconds(retry_requests_delay_ms)
    };

    context->fleet_api_client = std::make_shared<bringauto::fleet_protocol::http_client::FleetApiClient>(
        fleet_api_config, request_frequency_guard_config
    );

    context->last_command_timestamp = 0;
    return context;
}

int destroy(void **context) {
    if(*context == nullptr){
        return NOT_OK;
    }
    const auto con = reinterpret_cast<struct bamm::Context **> (context);

    delete *con;
    *con = nullptr;
    return OK;
}

int forward_status(const buffer device_status, const device_identification device, void *context) {
    if(context == nullptr){
        return CONTEXT_INCORRECT;
    }
    
    const auto con = static_cast<struct bamm::Context *> (context);

    if(device.device_type == bamm::AUTONOMY_DEVICE_TYPE) {
        const bringauto::fleet_protocol::cxx::BufferAsString device_status_bas(&device_status);
        const auto device_status_str = std::string(device_status_bas.getStringView());
        if (bringauto::protobuf::ProtobufHelper::validateAutonomyStatus(device_status_str) != OK) {
            return NOT_OK;
        }

        const bringauto::fleet_protocol::cxx::BufferAsString device_role(&device.device_role);
        const bringauto::fleet_protocol::cxx::BufferAsString device_name(&device.device_name);
        con->fleet_api_client->setDeviceIdentification(
            bringauto::fleet_protocol::cxx::DeviceID(
                device.module,
                device.device_type,
                0, //priority
                std::string(device_role.getStringView()),
                std::string(device_name.getStringView())
            )
        );

        try {
            con->fleet_api_client->sendStatus(device_status_str);
        } catch (std::exception&) {
            return NOT_OK;
        }

        return OK;
    }

    return NOT_OK;
}

int forward_error_message(const buffer error_msg, const device_identification device, void *context) {
    if(context == nullptr){
        return CONTEXT_INCORRECT;
    }

    const auto con = static_cast<struct bamm::Context *> (context);

    if(device.device_type == bamm::AUTONOMY_DEVICE_TYPE) {
        const bringauto::fleet_protocol::cxx::BufferAsString error_msg_bas(&error_msg);
        const auto error_msg_str = std::string(error_msg_bas.getStringView());
        if (bringauto::protobuf::ProtobufHelper::validateAutonomyError(error_msg_str) != OK) {
            return NOT_OK;
        }

        const bringauto::fleet_protocol::cxx::BufferAsString device_role(&device.device_role);
        const bringauto::fleet_protocol::cxx::BufferAsString device_name(&device.device_name);
        con->fleet_api_client->setDeviceIdentification(
            bringauto::fleet_protocol::cxx::DeviceID(
                device.module,
                device.device_type,
                0, //priority
                std::string(device_role.getStringView()),
                std::string(device_name.getStringView())
            )
        );

        try {
            con->fleet_api_client->sendStatus(
                error_msg_str, bringauto::fleet_protocol::http_client::FleetApiClient::StatusType::STATUS_ERROR
            );
        } catch (std::exception&) {
            return NOT_OK;
        }

        return OK;
    }
    
    return NOT_OK;
}

int device_disconnected(const int disconnect_type, const device_identification device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    const auto con = static_cast<struct bamm::Context *> (context);

    const std::string_view device_device_role(static_cast<char*> (device.device_role.data), device.device_role.size_in_bytes);
    const std::string_view device_device_name(static_cast<char*> (device.device_name.data), device.device_name.size_in_bytes);

    for(auto it = con->devices.begin(); it != con->devices.end(); ++it) {
        const std::string_view it_device_role(static_cast<char*> (it->device_role.data), it->device_role.size_in_bytes);
        const std::string_view it_device_name(static_cast<char*> (it->device_name.data), it->device_name.size_in_bytes);

        const bool device_is_present = it->device_type == device.device_type && it_device_role == device_device_role &&
            it_device_name == device_device_name && it->module == device.module && it->priority == device.priority;

        if(device_is_present) {
            deallocate(&it->device_role);
            deallocate(&it->device_name);
            con->devices.erase(it);
            return OK;
        }
    }

    return NOT_OK;
}

int device_connected(const device_identification device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    const auto con = static_cast<struct bamm::Context *> (context);

    device_identification new_device {
        .module = device.module,
        .device_type = device.device_type,
        .priority = device.priority
    };

    if(allocate(&new_device.device_role, device.device_role.size_in_bytes) != OK) {
        return NOT_OK;
    }
    std::memcpy(new_device.device_role.data, device.device_role.data, new_device.device_role.size_in_bytes);

    if(allocate(&new_device.device_name, device.device_name.size_in_bytes) != OK) {
        return NOT_OK;
    }
    std::memcpy(new_device.device_name.data, device.device_name.data, new_device.device_name.size_in_bytes);

    con->devices.emplace_back(new_device);
    return OK;
}

int wait_for_command(int timeout_time_in_ms, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    const auto con = static_cast<struct bamm::Context *> (context);
    std::unique_lock lock(con->mutex);
    std::pair<std::vector<std::shared_ptr<org::openapitools::client::model::Message>>,
        bringauto::fleet_protocol::http_client::FleetApiClient::ReturnCode> commands;
    bool parse_commands = con->last_command_timestamp != 0;
    
    try {
        commands = con->fleet_api_client->getCommands(con->last_command_timestamp + 1, true);
    } catch (std::exception&) {
        return TIMEOUT_OCCURRED;
    }

    bool received_no_commands = true;
    for(const auto& command : commands.first) {
        if(command->getTimestamp() > con->last_command_timestamp) {
            con->last_command_timestamp = command->getTimestamp();
        }

        const auto received_device_id = command->getDeviceId();
        if(received_device_id->getModuleId() == bamm::MISSION_MODULE_NUMBER) {
            received_no_commands = false;
        } else {
            continue;
        }

        if(parse_commands) {
            std::string command_str = command->getPayload()->getData()->getJson().serialize();
            if (bringauto::protobuf::ProtobufHelper::validateAutonomyCommand(command_str) != OK) {
                return NOT_OK;
            }

            con->command_vector.emplace_back(command_str, bringauto::fleet_protocol::cxx::DeviceID(
                received_device_id->getModuleId(),
                received_device_id->getType(),
                0, //priority
                received_device_id->getRole(),
                received_device_id->getName()
            ));
        }
    }

    if(received_no_commands && !parse_commands) {
        con->last_command_timestamp = 1;
    }

    if(received_no_commands || !parse_commands) {
        return TIMEOUT_OCCURRED;
    }
    return OK;
}

int pop_command(buffer* command, device_identification* device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    const auto con = static_cast<struct bamm::Context *> (context);
    const auto command_object = std::get<0>(con->command_vector.back());

    bringauto::fleet_protocol::cxx::StringAsBuffer::createBufferAndCopyData(command, command_object);

    const auto& device_id = std::get<1>(con->command_vector.back());

    device->module = device_id.getDeviceId().module;
    device->device_type = device_id.getDeviceId().device_type;
    device->priority = device_id.getDeviceId().priority;

    if(allocate(&device->device_role, device_id.getDeviceId().device_role.size_in_bytes) == NOT_OK) {
        return NOT_OK;
    }
    std::memcpy(device->device_role.data, device_id.getDeviceId().device_role.data, device->device_role.size_in_bytes);

    if(allocate(&device->device_name, device_id.getDeviceId().device_name.size_in_bytes) == NOT_OK) {
        return NOT_OK;
    }
    std::memcpy(device->device_name.data, device_id.getDeviceId().device_name.data, device->device_name.size_in_bytes);

    con->command_vector.pop_back();

    return static_cast<int> (con->command_vector.size());
}

int command_ack(const buffer command, const device_identification device, void *context) {
    return OK;
}
