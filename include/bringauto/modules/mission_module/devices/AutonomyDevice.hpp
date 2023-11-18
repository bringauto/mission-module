#pragma once
#include <modules/MissionModule.pb.h>

class AutonomyDevice {
public:
	 MissionModule::AutonomyCommand generate_command(std::vector<MissionModule::Station> stops, std::string route, MissionModule::AutonomyCommand::Action action);
};
