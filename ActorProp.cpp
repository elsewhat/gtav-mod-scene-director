#include "ActorProp.h"
#include <vector>
#include <algorithm>

std::vector<ActorProp> gtaActorProps = {
	{"Guitar","prop_acc_guitar_01",28252 },
	{"Barbell", "Prop_Barbell_02",61163}
};

ActorProp getDefaultActorProp() {
	return gtaActorProps[0];
}

ActorProp getNextActorProp(ActorProp prop) {
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(gtaActorProps.begin(), gtaActorProps.end(), [=](ActorProp const& actorProp) {
		return (prop.name == actorProp.name);
	}) - gtaActorProps.begin();
	if (foundIndex +1 >= gtaActorProps.size()) {//color not found or in last element
		return gtaActorProps[0];
	}
	else {
		return gtaActorProps[foundIndex +1];
	}	
}


