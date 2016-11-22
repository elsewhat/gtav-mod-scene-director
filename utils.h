/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#pragma once

#include <string>
#include "..\..\inc\types.h"

// returns module load path with trailing slash
std::string GetCurrentModulePath();
std::string roundNumber(float number);


class MathUtils {
public:
	static Vector3 rotationToDirection(Vector3 rotation);
	static Vector3 crossProduct(Vector3 a, Vector3 b);
};