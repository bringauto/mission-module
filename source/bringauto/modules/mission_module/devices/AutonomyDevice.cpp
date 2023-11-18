#include <bringauto/modules/mission_module/devices/AutonomyDevice.hpp>



MissionModule::AutonomyCommand AutonomyDevice::generate_command(std::vector<MissionModule::Station> stops, std::string route,
																MissionModule::AutonomyCommand::Action action) {
	MissionModule::AutonomyCommand command;
	auto stopsField = command.mutable_stops();
	stopsField->Add(stops.begin(), stops.end());
	command.set_route(route);
	command.set_action(action);
	return command;
}
