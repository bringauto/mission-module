#include <bringauto/json/EnumUtils.hpp>


namespace bringauto::json {

AutonomyState EnumUtils::stringToAutonomyState(const std::string &state) {
	if (state == "IDLE") {
		return AutonomyState::IDLE;
	} else if (state == "DRIVE") {
		return AutonomyState::DRIVE;
	} else if (state == "IN_STOP") {
		return AutonomyState::IN_STOP;
	} else if (state == "OBSTACLE") {
		return AutonomyState::OBSTACLE;
	} else if (state == "ERROR") {
		return AutonomyState::ERROR;
	}
	return AutonomyState::ERROR;
}

std::string EnumUtils::autonomyStateToString(const AutonomyState &state) {
	switch (state) {
	case AutonomyState::IDLE:
		return "IDLE";
	case AutonomyState::DRIVE:
		return "DRIVE";
	case AutonomyState::IN_STOP:
		return "IN_STOP";
	case AutonomyState::OBSTACLE:
		return "OBSTACLE";
	case AutonomyState::ERROR:
		return "ERROR";
	}
	return "ERROR";
}

AutonomyAction EnumUtils::stringToAutonomyAction(const std::string &action) {
	if (action == "NO_ACTION") {
		return AutonomyAction::NO_ACTION;
	} else if (action == "STOP") {
		return AutonomyAction::STOP;
	} else if (action == "START") {
		return AutonomyAction::START;
	}
	return AutonomyAction::NO_ACTION;
}

std::string EnumUtils::autonomyActionToString(const AutonomyAction &action) {
	switch (action) {
	case AutonomyAction::NO_ACTION:
		return "NO_ACTION";
	case AutonomyAction::STOP:
		return "STOP";
	case AutonomyAction::START:
		return "START";
	}
	return "NO_ACTION";
}

}