#pragma once
#include <vector>
struct DrivingMode
{
	int value;
	std::string name;
	bool useVehicleMaxSpeed;
	float manualMaxSpeed;
};

enum DRIVING_FLAGS
{
	FLAG1_STOP_VEHS=1,
	FLAG2_STOP_PEDS=2,
	FLAG3_AVOID_VEHS=4,
	FLAG5_AVOID_PEDS=16,
	FLAG6_AVOID_OBJS=32,
	FLAG8_STOP_LIGHTS=128,
	FLAG11_REVERSE=1024,
	FLAG19_SHORTEST_PATH = 262144,
	FLAG23_IGN_ROADS=4194304,
	FLAG25_IGN_PATHING = 16777216,
};

std::vector<DrivingMode> getAllDrivingModes();
DrivingMode getDefaultDrivingMode();
DrivingMode getNextDrivingMode(DrivingMode drivingMode);