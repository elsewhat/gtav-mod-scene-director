#include "relationship.h"
#include <vector>

std::vector<RelationshipGroup> sceneDirectorRelationshipGroups= {
	{ "ACTOR_GROUPA", "Group A", 0x5F0783F1 },
	{ "ACTOR_GROUPB", "Group B", 0x5F0783F2 },
	{ "ACTOR_GROUPC", "Group C", 0x5F0783F3 },
};

std::vector<RelationshipGroup> getAllRelationshipGroups(){
	return sceneDirectorRelationshipGroups;
}