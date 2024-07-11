#pragma once

#include <bringauto/json/Structures.hpp>

namespace bringauto::json {

class EnumUtils {
public:
	EnumUtils() = delete;

	static AutonomyState stringToAutonomyState(const std::string &state);

	static std::string autonomyStateToString(const AutonomyState &state);

	static AutonomyAction stringToAutonomyAction(const std::string &action);

	static std::string autonomyActionToString(const AutonomyAction &action);

};

}