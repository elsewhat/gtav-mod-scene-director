#include "weather.h"
#include <vector>

std::vector<Weather> gtaWeather= {
	{"CLEAR" },
	{"EXTRASUNNY" },
	{"CLOUDS" },
	{"OVERCAST" },
	{ "RAIN" },
	{ "CLEARING" },
	{"THUNDER" },
	{"SMOG" },
	{"FOGGY" },
	{"XMAS" },
	{"BLIZZARD" },
	{"SNOWLIGHT" },
};

std::vector<Weather> getAllGTAWeather() {
	return gtaWeather;
}