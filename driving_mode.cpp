#include "driving_mode.h"
#include <vector>
#include <algorithm>

std::vector<DrivingMode> gtaDrivingModes= {
	{ 786469, "Aggressive", true, 0.0 },
	{ FLAG3_AVOID_VEHS|FLAG5_AVOID_PEDS|FLAG6_AVOID_OBJS, "Aggressive 2", true, 0.0 },
	{FLAG1_STOP_VEHS|FLAG2_STOP_PEDS|FLAG8_STOP_LIGHTS, "Careful", false, 15.0 },
	{ FLAG1_STOP_VEHS | FLAG2_STOP_PEDS | FLAG8_STOP_LIGHTS, "Cruise", false, 5.0 },
	{ FLAG1_STOP_VEHS | FLAG3_AVOID_VEHS, "Impatient", false, 30.0 },
	{ FLAG23_IGN_ROADS | FLAG6_AVOID_OBJS, "Offroad",true, 0.0 },
	{FLAG3_AVOID_VEHS|FLAG5_AVOID_PEDS|FLAG6_AVOID_OBJS|FLAG11_REVERSE, "Reverse", true,0.0 },
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