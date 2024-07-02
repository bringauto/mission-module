#ifndef SKIP_PROTOBUF
#include <MissionModule.pb.h>
#include <bringauto/protobuf/ProtobufHelper.hpp>
#include <google/protobuf/util/message_differencer.h>
#endif

#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>
#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>
#include <bringauto/modules/mission_module/Constants.hpp>




namespace bringauto::modules::mission_module::devices {

std::map<unsigned int, std::chrono::milliseconds> AutonomyDevice::last_sent_status_timestamps_ {};

int AutonomyDevice::send_status_condition(const struct buffer current_status, const struct buffer new_status, unsigned int device_type) {
#ifndef SKIP_PROTOBUF
	auto currentAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(current_status);
	auto newAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(new_status);

	if (currentAutonomyStatus.state() != newAutonomyStatus.state()
		|| !google::protobuf::util::MessageDifferencer::Equals(currentAutonomyStatus.nextstop(), newAutonomyStatus.nextstop())) {
		return OK;
	} else if (newAutonomyStatus.telemetry().speed() >= settings::status_speed_threshold) {
		auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		if (last_sent_status_timestamps_[device_type] + settings::status_sending_period < current_time) {
			last_sent_status_timestamps_[device_type] = current_time;
			return OK;
		}
	}
	return CONDITION_NOT_MET;
#else
	return OK;
#endif
}

int AutonomyDevice::generate_command(struct buffer *generated_command, const struct buffer new_status,
									 const struct buffer current_status, const struct buffer current_command) {
#ifndef SKIP_PROTOBUF
	auto currentAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(current_status);
	auto newAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(new_status);
	auto currentAutonomyCommand = protobuf::ProtobufHelper::parseAutonomyCommand(current_command);

	if (currentAutonomyStatus.state() == MissionModule::AutonomyStatus_State_DRIVE && newAutonomyStatus.state() == MissionModule::AutonomyStatus_State_IN_STOP) {
		auto* stations = currentAutonomyCommand.mutable_stops();
		if (stations->size() > 0) {
			stations->erase(stations->begin());
		}
	}
	return protobuf::ProtobufHelper::serializeProtobufMessageToBuffer(generated_command, currentAutonomyCommand);
#else
	if (allocate(generated_command, current_command.size_in_bytes) != OK) {
		return NOT_OK;
	}
	std::memcpy(generated_command->data, current_command.data, generated_command->size_in_bytes);
	return OK;
#endif
}

int AutonomyDevice::aggregate_status(struct buffer *aggregated_status, const struct buffer current_status,
									 const struct buffer new_status) {
	if (allocate(aggregated_status, new_status.size_in_bytes) != OK) {
		return NOT_OK;
	}
	std::memcpy(aggregated_status->data, new_status.data, aggregated_status->size_in_bytes);
	return OK;
}

int AutonomyDevice::aggregate_error(struct buffer *error_message, const struct buffer current_error_message,
									const struct buffer status) {
#ifndef SKIP_PROTOBUF
	auto autonomyError = protobuf::ProtobufHelper::parseAutonomyError(current_error_message);
	auto autonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(status);

	if (autonomyStatus.state() == MissionModule::AutonomyStatus_State_IN_STOP) {
		const auto & finishedStops = autonomyError.finishedstops();
		if (!finishedStops.empty()) {
			if (!google::protobuf::util::MessageDifferencer::Equals(finishedStops[finishedStops.size() -1], autonomyStatus.nextstop())) {
				autonomyError.add_finishedstops()->CopyFrom(autonomyStatus.nextstop());
			}
		} else {
			autonomyError.add_finishedstops()->CopyFrom(autonomyStatus.nextstop());
		}
	}

	return protobuf::ProtobufHelper::serializeProtobufMessageToBuffer(error_message, autonomyError);
#else
	if (allocate(error_message, current_error_message.size_in_bytes) != OK) {
		return NOT_OK;
	}
	std::memcpy(error_message->data, current_error_message.data, error_message->size_in_bytes);
	return OK;
#endif
}

int AutonomyDevice::generate_first_command(struct buffer *default_command) {
#ifndef SKIP_PROTOBUF
	MissionModule::AutonomyCommand command = generateCommand(std::vector<MissionModule::Station>(), "", MissionModule::AutonomyCommand_Action_NO_ACTION);
	if (protobuf::ProtobufHelper::serializeProtobufMessageToBuffer(default_command, command) != OK) {
		return NOT_OK;
	}
	return OK;
#else
	std::string default_command_str = "{}";
	if ((allocate(default_command, default_command_str.length())) == OK) {
		std::memcpy(default_command->data, default_command_str.c_str(), default_command->size_in_bytes);
		return OK;
	}
	return NOT_OK;
#endif
}

int AutonomyDevice::status_data_valid(const struct buffer status) {
#ifndef SKIP_PROTOBUF
	try {
		protobuf::ProtobufHelper::parseAutonomyStatus(status);
	}
	catch (...) {
		return NOT_OK;
	}
#endif
	return OK;
}

int AutonomyDevice::command_data_valid(const struct buffer command) {
#ifndef SKIP_PROTOBUF
	try {
		protobuf::ProtobufHelper::parseAutonomyCommand(command);
	}
	catch (...) {
		return NOT_OK;
	}
#endif
	return OK;
}

#ifndef SKIP_PROTOBUF
MissionModule::AutonomyCommand
AutonomyDevice::generateCommand(std::vector<MissionModule::Station> stops, std::string route,
								MissionModule::AutonomyCommand::Action action) {
	MissionModule::AutonomyCommand command;
	auto stopsField = command.mutable_stops();
	stopsField->Add(stops.begin(), stops.end());
	command.set_route(route);
	command.set_action(action);
	return command;
}
#endif

}