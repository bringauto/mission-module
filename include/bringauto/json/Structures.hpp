#pragma once

#include <list>
#include <string>

namespace bringauto::json {

enum AutonomyState {
	IDLE, DRIVE, IN_STOP, OBSTACLE, ERROR
};

enum AutonomyAction {
	NO_ACTION, STOP, START
};

struct Position {
	double alt;
	double lat;
	double lon;

	double altitude() { return alt; };
	double latitude() { return lat; };
	double longitude() { return lon; };

	bool operator==(const Position &other) const {
		return alt == other.alt && lat == other.lat && lon == other.lon;
	};
};

struct Station {
	std::string nam;
	Position pos;

	std::string name() { return nam; };
	Position position() { return pos; };

	bool operator==(const Station &other) const {
		return nam == other.nam && pos == other.pos;
	};
};

struct Telemetry {
	double spe;
	double fue;
	Position pos;

	double speed() { return spe; };
	double fuel() { return fue; };
	Position position() { return pos; };
};

struct AutonomyStatus {
	Telemetry tel;
	AutonomyState sta;
	Station nex;

	Telemetry telemetry() { return tel; };
	AutonomyState state() { return sta; };
	Station nextstop() { return nex; };
};

struct AutonomyCommand {
	std::list<Station> sto;
	std::string rou;
	AutonomyAction act;

	std::list<Station> stops() { return sto; };
	std::string route() { return rou; };
	AutonomyAction action() { return act; };
};

struct AutonomyError {
	std::list<Station> fin;

	std::list<Station> finishedstops() { return fin; };
};

}