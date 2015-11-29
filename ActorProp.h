#pragma once
#include <vector>

struct ActorProp {
	std::string name;
	char* propId;
	int bone;
};



std::vector<ActorProp> getActorProps();
ActorProp getDefaultActorProp();
ActorProp getNextActorProp(ActorProp actorProp);