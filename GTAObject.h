#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include "utils.h"
#include "animation.h"
#include "actor.h"


struct GTAObject{
	std::string title;
	char* objName;
	Hash objHash;
	Object objReference;
	Ped objectOwner;

	bool isNull() {
		return objHash == 0;
	}
};

GTAObject getGTAObjectFromObjName(std::string objName);