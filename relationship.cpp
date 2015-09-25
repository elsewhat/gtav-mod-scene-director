#include "relationship.h"
#include <vector>
#include <algorithm>

std::vector<RelationshipGroup> sceneDirectorRelationshipGroups= {
	{ "ACTOR_GROUPA", "Group A", 0x5F0783F1 },
	{ "ACTOR_GROUPB", "Group B", 0x5F0783F2 },
	{ "ACTOR_GROUPC", "Group C", 0x5F0783F3 },
};

std::vector<RelationshipGroup> getAllRelationshipGroups(){
	return sceneDirectorRelationshipGroups;
}

RelationshipGroup getDefaultRelationshipGroup() {
	return sceneDirectorRelationshipGroups[0];
}

RelationshipGroup getNextRelationshipGroup(RelationshipGroup relationshipGroup) {

	//int foundIndex = find(spotLightColors.begin(), spotLightColors.end(), spotLightColor) - spotLightColors.begin();
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(sceneDirectorRelationshipGroups.begin(), sceneDirectorRelationshipGroups.end(), [=](RelationshipGroup const& group) {
		return (group.actorHash == relationshipGroup.actorHash);
	}) - sceneDirectorRelationshipGroups.begin();
	if (foundIndex + 1 >= sceneDirectorRelationshipGroups.size()) {//color not found or in last element
		return sceneDirectorRelationshipGroups[0];
	}
	else {
		return sceneDirectorRelationshipGroups[foundIndex + 1];
	}
}