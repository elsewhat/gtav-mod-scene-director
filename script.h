/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#pragma once

#include "..\..\inc\natives.h"
#include "..\..\inc\types.h"
#include "..\..\inc\enums.h"

#include "..\..\inc\main.h"

void ScriptMain();

enum SCENE_MODE {
	SCENE_MODE_ACTIVE = 1,
	SCENE_MODE_SETUP = 0
};

enum VEHICLE_TYPE {
	VEHICLE_TYPE_CAR,
	VEHICLE_TYPE_HELI,
	VEHICLE_TYPE_PLANE,
	VEHICLE_TYPE_BOAT,
	VEHICLE_TYPE_SUB,
	VEHICLE_TYPE_BIKE
};