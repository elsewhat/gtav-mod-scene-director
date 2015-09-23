#pragma once
#include <vector>
enum SPOT_LIGHT_TYPE
{
	SPOT_LIGHT_NONE = 0,
	SPOT_LIGHT_ACTOR_ABOVE = 1,
	SPOT_LIGHT_ACTOR_LEFT = 2,
	SPOT_LIGHT_ACTOR_RIGHT = 3,
	SPOT_LIGHT_ACTOR_INFRONT = 4,
	SPOT_LIGHT_ACTOR_BEHIND=5,
};

struct SpotLightColor {
	std::string name;
	int r, g, b;
};

const SPOT_LIGHT_TYPE last_spot_light_type = SPOT_LIGHT_ACTOR_BEHIND;

std::string getNameForSpotLightType(SPOT_LIGHT_TYPE type);

std::vector<SpotLightColor> getSpotLightColors();
SpotLightColor getDefaultSpotLightColor();
SpotLightColor getNextSpotLightColor(SpotLightColor spotLightColor);