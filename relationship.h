#pragma once
#include <vector>
#include "..\..\inc\types.h"

struct RelationshipGroup
{
	char* id;
	char* name;
	DWORD actorHash;
};

std::vector<RelationshipGroup> getAllRelationshipGroups();