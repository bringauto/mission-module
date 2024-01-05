#include <external_server_interface.h>
#include <bringauto/modules/mission_module/external_server_api_structures.hpp>
#include <bringauto/modules/mission_module/MissionModule.hpp>
#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>
#include <bringauto/protobuf/ProtobufHelper.hpp>
#include <CppRestOpenAPIClient/api/DeviceApi.h>
#include <bringauto/fleet_protocol/cxx/DeviceID.hpp>
#include <bringauto/fleet_protocol/cxx/KeyValueConfig.hpp>
#include <bringauto/fleet_protocol/cxx/StringAsBuffer.hpp>
#include <google/protobuf/util/json_util.h>
 
#include <vector>
#include <cstring>
#include <iostream>
#include <condition_variable>


using namespace org::openapitools::client;
namespace bamm = bringauto::modules::mission_module;

void *init(const config config_data) {
    auto *context = new struct bamm::context;
    bringauto::fleet_protocol::cxx::KeyValueConfig config(config_data);

    for (auto i = config.cbegin(); i != config.cend(); i++) {
        if (i->first == "api_url") {
            context->api_url = i->second;
        }
        else if (i->first == "api_key") {
            context->api_key = i->second;
        }
        else if (i->first == "company_name") {
            context->company_name = i->second;
        }
        else if (i->first == "car_name") {
            context->car_name = i->second;
        }
    }

    context->last_command_timestamp = 0;
    return context;
}

int destroy(void **context) {
    if(*context == nullptr){
        return NOT_OK;
    }
    auto con = reinterpret_cast<struct bamm::context **> (context);

    delete *con;
    *con = nullptr;
    return OK;
}

int forward_status(const buffer device_status, const device_identification device, void *context) {
    if(context == nullptr){
        return CONTEXT_INCORRECT;
    }
    
    auto con = static_cast<struct bamm::context *> (context);
    std::unique_lock lock(con->mutex);

    if(device.device_type == bamm::AUTONOMY_DEVICE_TYPE) {
        auto api_config_ptr = std::make_shared<api::ApiConfiguration>();
        api_config_ptr->setBaseUrl(con->api_url);
        api_config_ptr->setApiKey("api_key", con->api_key);
        auto api_client_ptr = std::make_shared<api::ApiClient>();
        
        api_client_ptr->setConfiguration(api_config_ptr);
        api::DeviceApi device_api(api_client_ptr);
        
        auto status_ptr = std::make_shared<model::Message>();
        status_ptr->setTimestamp(utility::datetime::utc_timestamp());
        
        auto device_id_ptr = std::make_shared<model::DeviceId>();
        device_id_ptr->setModuleId(device.module);
        device_id_ptr->setType(device.device_type);
        bringauto::fleet_protocol::cxx::BufferAsString device_role(&device.device_role);
        device_id_ptr->setRole(std::string(device_role.getStringView()));
        bringauto::fleet_protocol::cxx::BufferAsString device_name(&device.device_name);
        device_id_ptr->setName(std::string(device_name.getStringView()));
        status_ptr->setDeviceId(device_id_ptr);
        
        auto payload_ptr = std::make_shared<model::Payload>();
        payload_ptr->setMessageType("STATUS");
        payload_ptr->setEncoding("JSON");
        auto payload_data_ptr = std::make_shared<model::Payload_data>();
        std::string device_status_str;
        auto device_status_parsed = bringauto::protobuf::ProtobufHelper::parseAutonomyStatus(device_status);
        google::protobuf::util::MessageToJsonString(device_status_parsed, &device_status_str);
        auto payload_data_json = web::json::value::parse(device_status_str);
        payload_data_ptr->setJson(payload_data_json);
        payload_ptr->setData(payload_data_ptr);
        status_ptr->setPayload(payload_ptr);
        
        std::vector<std::shared_ptr<model::Message>> statuses;
        statuses.push_back(status_ptr);

        try {
            auto task = device_api.sendStatuses(con->company_name, con->car_name, statuses);
            task.wait();
        } catch (std::exception& e) {
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
    
    return OK;
}

int device_disconnected(const int disconnect_type, const device_identification device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bamm::context *> (context);

    const std::string_view device_device_role(static_cast<char*> (device.device_role.data), device.device_role.size_in_bytes);
    const std::string_view device_device_name(static_cast<char*> (device.device_name.data), device.device_name.size_in_bytes);

    for(auto it = con->devices.begin(); it != con->devices.end(); it++) {
        const std::string_view it_device_role(static_cast<char*> (it->device_role.data), it->device_role.size_in_bytes);
        const std::string_view it_device_name(static_cast<char*> (it->device_name.data), it->device_name.size_in_bytes);

        bool device_is_present = it->device_type == device.device_type && it_device_role == device_device_role && it_device_name == device_device_name
                                 && it->module == device.module && it->priority == device.priority;

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

    auto con = static_cast<struct bamm::context *> (context);

    device_identification new_device;

    new_device.module = device.module;
    new_device.device_type = device.device_type;
    new_device.priority = device.priority;

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

    auto con = static_cast<struct bamm::context *> (context);
    std::unique_lock lock(con->mutex);
    
    auto api_config_ptr = std::make_shared<api::ApiConfiguration>();
    api_config_ptr->setBaseUrl(con->api_url);
    api_config_ptr->setApiKey("api_key", con->api_key);
    auto api_client_ptr = std::make_shared<api::ApiClient>();
    api_client_ptr->setConfiguration(api_config_ptr);
    api::DeviceApi device_api(api_client_ptr);
    std::vector<std::shared_ptr<model::Message>> commands;
    
    try {
        auto commands_request = device_api.listCommands(con->company_name, con->car_name, false, con->last_command_timestamp + 1, true);
        commands = commands_request.get();
    } catch (std::exception& e) {
        return TIMEOUT_OCCURRED;
    }

    for(auto command : commands) {
        auto received_device_id = command->getDeviceId();
        MissionModule::AutonomyCommand proto_command {};
        google::protobuf::util::JsonStringToMessage(command->getPayload()->getData()->getJson().serialize(), &proto_command);
        std::string command_str;
        proto_command.SerializeToString(&command_str);

        con->command_vector.emplace_back(command_str, bringauto::fleet_protocol::cxx::DeviceID(
            received_device_id->getModuleId(),
            received_device_id->getType(),
            0, //priority
            received_device_id->getRole(),
            received_device_id->getName()
        ));

        if(command->getTimestamp() > con->last_command_timestamp) {
            con->last_command_timestamp = command->getTimestamp();
        }
    }

    if(commands.empty()) {
        return TIMEOUT_OCCURRED;
    }
    else {
        return OK;
    }
}

int pop_command(buffer* command, device_identification* device, void *context) {
    if(context == nullptr) {
        return CONTEXT_INCORRECT;
    }

    auto con = static_cast<struct bamm::context *> (context);
    auto command_object = std::get<0>(con->command_vector.back());

    bringauto::fleet_protocol::cxx::StringAsBuffer::createBufferAndCopyData(command, command_object);
    /*if(command_object.serializeToBuffer(command) == NOT_OK) {
        return NOT_OK;
    }*/

    auto& device_id = std::get<1>(con->command_vector.back());

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