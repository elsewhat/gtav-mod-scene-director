#pragma once
#include <string>
#include <vector>
#include "..\..\inc\types.h"

struct Prop
{
	Object objectReference;
	Hash modelHash;
	Vector3 location;
	std::string modelName;
	bool isVisible;
	bool doCollision;

	std::string toString() {
		return std::to_string(objectReference) + " " + std::string(modelName);
	}
};
