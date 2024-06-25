#include <MissionModule.pb.h>
#include <fleet_protocol/module_maintainer/module_gateway/module_manager.h>

#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>
#include <bringauto/modules/mission_module/Constants.hpp>
#include <bringauto/protobuf/ProtobufHelper.hpp>
#include <google/protobuf/util/message_differencer.h>



namespace bringauto::modules::mission_module::devices {

std::map<unsigned int, std::chrono::milliseconds> AutonomyDevice::last_sent_status_timestamps_ {};

int AutonomyDevice::send_status_condition(const struct buffer current_status, const struct buffer new_status, unsigned int device_type) {
	auto currentAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(current_status);
	auto newAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(new_status);

	if (currentAutonomyStatus.state() != newAutonomyStatus.state()
		|| !google::protobuf::util::MessageDifferencer::Equals(currentAutonomyStatus.nextstop(), newAutonomyStatus.nextstop())) {
		return OK;
	} else if (newAutonomyStatus.telemetry().speed() >= bringauto::modules::mission_module::status_speed_threshold) {
		auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		if (last_sent_status_timestamps_[device_type] + bringauto::modules::mission_module::status_sending_period < current_time) {
			last_sent_status_timestamps_[device_type] = current_time;
			return OK;
		}
	}
	return CONDITION_NOT_MET;
}

int AutonomyDevice::generate_command(struct buffer *generated_command, const struct buffer new_status,
									 const struct buffer current_status, const struct buffer current_command) {
	auto currentAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(current_status);
	auto newAutonomyStatus = protobuf::ProtobufHelper::parseAutonomyStatus(new_status);
	auto currentAutonomyCommand = protobuf::ProtobufHelper::parseAutonomyCommand(current_command);

	if (currentAutonomyStatus.state() == MissionModule::AutonomyStatus_State_DRIVE &&
			newAutonomyStatus.state() == MissionModule::AutonomyStatus_State_IN_STOP) {
		auto* stations = currentAutonomyCommand.mutable_stops();
		if (stations->size() > 0) {
			stations->erase(stations->begin());
		}
	}
	return protobuf::ProtobufHelper::serializeProtobufMessageToBuffer(generated_command, currentAutonomyCommand);
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
}

int AutonomyDevice::generate_first_command(struct buffer *default_command) {
	MissionModule::AutonomyCommand command = generateCommand(std::vector<MissionModule::Station>(), "", MissionModule::AutonomyCommand_Action_NO_ACTION);
	if (protobuf::ProtobufHelper::serializeProtobufMessageToBuffer(default_command, command) != OK) {
		return NOT_OK;
	}
	return OK;
}

int AutonomyDevice::status_data_valid(const struct buffer status) {
	try {
		protobuf::ProtobufHelper::parseAutonomyStatus(status);
	}
	catch (...) {
		return NOT_OK;
	}
	return OK;
}

int AutonomyDevice::command_data_valid(const struct buffer command) {
	try {
		protobuf::ProtobufHelper::parseAutonomyCommand(command);
	}
	catch (...) {
		return NOT_OK;
	}
	return OK;
}

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

}