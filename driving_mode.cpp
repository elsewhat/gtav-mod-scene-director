#include "driving_mode.h"
#include <vector>
#include <algorithm>

std::vector<DrivingMode> gtaDrivingModes= {
	{ FLAG25_IGN_PATHING, "Direct",true, 0.0 },
	{ FLAG25_IGN_PATHING | FLAG11_REVERSE, "Direct-Reverse", true,0.0 },
	{ 786469, "Road-Aggressive", true, 0.0 },
	{ FLAG3_AVOID_VEHS|FLAG5_AVOID_PEDS|FLAG6_AVOID_OBJS, "Road-Aggressive 2", true, 0.0 },
	{FLAG1_STOP_VEHS|FLAG2_STOP_PEDS|FLAG8_STOP_LIGHTS, "Road-Careful", false, 15.0 },
	{ FLAG1_STOP_VEHS | FLAG2_STOP_PEDS | FLAG8_STOP_LIGHTS, "Road-Cruise", false, 5.0 },
	{ FLAG1_STOP_VEHS | FLAG3_AVOID_VEHS, "Road-Impatient", false, 30.0 },
	{FLAG3_AVOID_VEHS|FLAG5_AVOID_PEDS|FLAG6_AVOID_OBJS|FLAG11_REVERSE, "Road-Reverse", true,0.0 },
	{ FLAG19_SHORTEST_PATH | FLAG6_AVOID_OBJS, "Offroad w/path",true, 0.0 },
};

std::vector<DrivingMode> getAllDrivingModes() {
	return gtaDrivingModes;
}

DrivingMode getDefaultDrivingMode() {
	return gtaDrivingModes[0];
}

DrivingMode getNextDrivingMode(DrivingMode drivingMode){
	//int foundIndex = find(spotLightColors.begin(), spotLightColors.end(), spotLightColor) - spotLightColors.begin();
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(gtaDrivingModes.begin(), gtaDrivingModes.end(), [=](DrivingMode const& compDrivingMode) {
		return (compDrivingMode.value == drivingMode.value && compDrivingMode.manualMaxSpeed == drivingMode.manualMaxSpeed);
	}) - gtaDrivingModes.begin();
	if (foundIndex + 1 >= gtaDrivingModes.size()) {//color not found or in last element
		return gtaDrivingModes[0];
	}
	else {
		return gtaDrivingModes[foundIndex + 1];
	}
}