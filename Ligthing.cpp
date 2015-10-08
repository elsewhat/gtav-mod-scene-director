#include "Lighting.h"
#include <vector>
#include <algorithm>

std::vector<SpotLightColor> spotLightColors = {
	{ "White",256,256,256 },
	{ "Silver",192,192,192 },
	{ "Gray" ,128,128,128 },
	{ "Maroon",128,0,0 },
	{ "Red",256,0,0 },
	{ "Purple",128,0,128 },
	{ "Fuchsia",256,0,256 },
	{ "Green", 0,128,0 },
	{ "Lime",0,256,0 },
	{ "Olive",128,128,0 },
	{ "Yellow",256,256,0 },
	{ "Navy",0,0,128 },
	{ "Blue",0,0,256 },
	{ "Teal",0,128,128 },
	{ "Aqua",0,256,256 },
};

std::vector<SpotLightColor> getSpotLightColors() {
	return spotLightColors;
}

SpotLightColor getDefaultSpotLightColor() {
	return SpotLightColor{ "White",256,256,256 };
}

SpotLightColor getNextSpotLightColor(SpotLightColor spotLightColor) {

	//int foundIndex = find(spotLightColors.begin(), spotLightColors.end(), spotLightColor) - spotLightColors.begin();
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(spotLightColors.begin(), spotLightColors.end(), [=](SpotLightColor const& color) {
		return (color.r == spotLightColor.r && color.g == spotLightColor.g && color.b == spotLightColor.b);
	}) - spotLightColors.begin();
	if (foundIndex +1 >= spotLightColors.size()) {//color not found or in last element
		return spotLightColors[0];
	}
	else {
		return spotLightColors[foundIndex +1];
	}	
}

std::string getNameForSpotLightType(SPOT_LIGHT_TYPE type) {
	switch (type)
	{
	case SPOT_LIGHT_NONE:
		return "None";
		break;
	case SPOT_LIGHT_ACTOR_ABOVE:
		return "Above";
		break;
	case SPOT_LIGHT_WEST:
		return "West";
		break;
	case SPOT_LIGHT_EAST:
		return "East";
		break;
	case SPOT_LIGHT_NORTH:
		return "North";
		break;
	case SPOT_LIGHT_SOUTH:
		return "South";
		break;
	default:
		return "Error";
		break;
	}
}

