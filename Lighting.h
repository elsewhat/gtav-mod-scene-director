#pragma once
#include <vector>
enum SPOT_LIGHT_TYPE
{
	SPOT_LIGHT_NONE = 0,
	SPOT_LIGHT_ACTOR_ABOVE = 1,
	SPOT_LIGHT_WEST = 2,
	SPOT_LIGHT_EAST = 3,
	SPOT_LIGHT_NORTH = 4,
	SPOT_LIGHT_SOUTH=5,
};

struct SpotLightColor {
	std::string name;
	int r, g, b;
};

const SPOT_LIGHT_TYPE last_spot_light_type = SPOT_LIGHT_SOUTH;

std::string getNameForSpotLightType(SPOT_LIGHT_TYPE type);

std::vector<SpotLightColor> getSpotLightColors();
SpotLightColor getDefaultSpotLightColor();
SpotLightColor getNextSpotLightColor(SpotLightColor spotLightColor);