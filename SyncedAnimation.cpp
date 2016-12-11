#include "SyncedAnimation.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

SyncedAnimation::SyncedAnimation()
{
	m_isNull = true;
}

SyncedAnimation::SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, float deltaZLocation)
{
	m_title = title;
	m_actorAnimations = actorAnimations;
	m_deltaZLocation = deltaZLocation;
	m_isNull = false;
}

SyncedAnimation::SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, std::vector<GTAObject>  syncObjects, float deltaZLocation)
{
	m_title = title;
	m_actorAnimations = actorAnimations;
	m_objectAnimations = objectAnimations;
	m_deltaZLocation = deltaZLocation;
	m_syncObjects = syncObjects;
	m_isNull = false;
}

void SyncedAnimation::executeSyncedAnimation(std::vector<Actor> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop)
{
	log_to_file("SyncedAnimation->executeSyncedAnimation " + toString());
	DWORD ticksStart = GetTickCount();
	
	//must be at least as many actors as animations
	for (int i=0; i < m_actorAnimations.size(); i++) {
		if (syncActors.size() <= i || syncActors[i].isNullActor()) {
			set_status_text("Must have at least " + std::to_string(m_actorAnimations.size()) + " actors");
			return;
		}
	}

	//must be at least as many objects as object animations
	for (int i = 0; i < m_objectAnimations.size(); i++) {
		if (m_syncObjects.size() <= i || m_syncObjects[i].isNull()) {
			set_status_text("Must have at least " + std::to_string(m_objectAnimations.size()) + " objects");
			return;
		}
	}


	//load actor animations
	for (auto &animation : m_actorAnimations) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
		log_to_file("Loading animation " + animation.toString());
		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
				set_status_text("Failed to load actor animations");
				return;
			}
		}
	}

	//load object animations
	for (auto &animation : m_objectAnimations) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
				set_status_text("Failed to load object animations");
				return;
			}
		}
	}

	log_to_file("About to load objects");
	//load objects
	for (auto &gtaObject : m_syncObjects) {
		if (gtaObject.objHash == -1) {
			gtaObject.objHash = GAMEPLAY::GET_HASH_KEY(gtaObject.objName);
		}

		STREAMING::REQUEST_MODEL(gtaObject.objHash);
		while (!STREAMING::HAS_MODEL_LOADED(gtaObject.objHash))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
				log_to_file("Failed to load GTAObject name:" + std::string(gtaObject.objName) + " Hash:" + std::to_string(gtaObject.objHash));
				set_status_text("Failed to load objects");
				return;
			}
		}
	}

	//Setup scene for synced animation
	Vector3 sceneLocation = directLocation;

	float startHeading = 0.0;
	if (syncActors.size() > 1 && !syncActors[0].isNullActor()) {
		startHeading = ENTITY::GET_ENTITY_HEADING(syncActors[0].getActorPed());
		if (useFirstActorLocation) {
			sceneLocation = ENTITY::GET_ENTITY_COORDS(syncActors[0].getActorPed(), true);
		}
	}
	log_to_file("About to create scene");
	m_sceneId = PED::CREATE_SYNCHRONIZED_SCENE(sceneLocation.x, sceneLocation.y, sceneLocation.z+m_deltaZLocation, 0.0, 0.0, startHeading, 2);
	PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, false);

	log_to_file("About to add animations for actors");
	//Add the animations to the scene
	int actorIndex = 0;
	for (auto &animation : m_actorAnimations) {
		AI::TASK_SYNCHRONIZED_SCENE(syncActors[actorIndex].getActorPed(), m_sceneId, animation.animLibrary, animation.animName, 1000.0, -4.0, 64, 0, 0x447a0000, 0);
		m_pedsInScene.push_back(syncActors[actorIndex].getActorPed());

		m_pedsInSceneStartLocation.push_back(ENTITY::GET_ENTITY_COORDS(syncActors[actorIndex].getActorPed(), true));
		m_pedsInSceneStartHeading.push_back(ENTITY::GET_ENTITY_HEADING(syncActors[actorIndex].getActorPed()));

		actorIndex++;
	}

	log_to_file("About to add animations for objects");
	int objectIndex = 0;
	for (auto &animation : m_objectAnimations) {
		//create object if it doesn't exist
		if (m_syncObjects[objectIndex].objReference == 0) {
			log_to_file("About to create object with name " + std::string(m_syncObjects[objectIndex].objName) + " and hash " + std::to_string(m_syncObjects[objectIndex].objHash));
			int newObjectRef = OBJECT::CREATE_OBJECT(m_syncObjects[objectIndex].objHash, sceneLocation.x, sceneLocation.y, sceneLocation.z, true, true, false);
			m_syncObjects[objectIndex].objReference = newObjectRef;
			m_objectsInScene.push_back(newObjectRef);
		}
		log_to_file("Object animation objReference:" + std::to_string(m_syncObjects[objectIndex].objReference) + " Animation " + animation.animName);
		ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(m_syncObjects[objectIndex].objReference, m_sceneId, animation.animName, animation.animLibrary, 1000.0, -4.0, 0,0x447a0000);
		objectIndex++;
	}

	if (doLoop) {
		PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, doLoop);
	}

	log_to_file("About to execute the synchronized scene");
	//execute the scene
	PED::SET_SYNCHRONIZED_SCENE_PHASE(m_sceneId, 0.0);
}



bool SyncedAnimation::isCompleted()
{
	if (m_sceneId != 0) {
		if (PED::IS_SYNCHRONIZED_SCENE_LOOPED(m_sceneId)) {
			return false;
		}
		float sceneStatus = PED::GET_SYNCHRONIZED_SCENE_PHASE(m_sceneId);
		if (sceneStatus>=1.0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}

}

void SyncedAnimation::cleanupAfterExecution(bool doDeleteObjects, bool teleportActorsBackToStart)
{
	log_to_file("SyncedAnimation cleanupAfterExecution ");
	int i = 0;
	for (auto &ped : m_pedsInScene) {
		//no idea what 2+3 param is
		ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(ped, 0.0, 1);

		if (teleportActorsBackToStart) {
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped, m_pedsInSceneStartLocation[i].x, m_pedsInSceneStartLocation[i].y, m_pedsInSceneStartLocation[i].z, 0, 0, 1);
			ENTITY::SET_ENTITY_HEADING(ped, m_pedsInSceneStartHeading[i]);
		}
		i++;
	}
	for (auto &obj : m_objectsInScene) {
		//no idea what 2+3 param is
		ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(obj, 0.0, 1);
		if (doDeleteObjects) {
			log_to_file("About to delete object");
			OBJECT::DELETE_OBJECT(&obj);
		}
	}
	

	log_to_file("SyncedAnimation trying to dispose scene ");
	//dispose scene
	PED::_0xCD9CC7E200A52A6F(m_sceneId);
	m_sceneId = 0;

}

void SyncedAnimation::setLooping(bool doLoop)
{
	if (m_sceneId!=0) {
		log_to_file("Setting looping for scene to " + doLoop);
		PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, doLoop);
	}
}


bool SyncedAnimation::matchesFilter(std::string filterStr)
{
	std::vector<std::string> filterTokens = StringUtils::split(filterStr, ' ');

	FILTER_OPERATORS currentFitlerOperator = FILTER_OR;
	bool matchesFilter = true;
	int nrMatches = 0;
	for (auto token : filterTokens) {
		if (!token.empty() && (token.compare("AND") == 0 || token.compare("and") == 0)) {
			currentFitlerOperator = FILTER_AND;
			continue;
		}
		else if (!token.empty() && (token.compare("OR") == 0 || token.compare("or") == 0)) {
			currentFitlerOperator = FILTER_OR;
			continue;
		}
		else if (!token.empty() && (token.compare("NOT") == 0 || token.compare("not") == 0)) {
			currentFitlerOperator = FILTER_NOT;
			continue;
		}

		if (currentFitlerOperator == FILTER_AND) {
			if (std::string(m_title).find(token) == std::string::npos) {
				return false;
			}
		}
		else if (currentFitlerOperator == FILTER_NOT) {
			if (std::string(m_title).find(token) != std::string::npos) {
				return false;
			}
		}
		else {//FILTER_OR
			if (std::string(m_title).find(token) != std::string::npos) {
				nrMatches++;
			}
		}
	}

	if (nrMatches > 0) {
		return true;
	}
	else {
		return false;
	}
}

bool SyncedAnimation::isNull()
{
	return m_isNull;
}

std::string SyncedAnimation::toString()
{
	std::string objString = m_title + " Anims:";
	for (auto &animation : m_actorAnimations) {
		objString += std::to_string(animation.shortcutIndex) + " ";
	}
	if (m_objectAnimations.size() > 0) {
		objString += "ObjectAnims:";
		for (auto &animation : m_objectAnimations) {
			objString += std::to_string(animation.shortcutIndex) + " ";
		}
	}

	objString += "Z-adjustment:" + roundNumber(m_deltaZLocation);


	return objString;
}

//Defined all SyncedAnimations identified so far

//SyncedAnimation aSyncedAnimation = SyncedAnimation("Test",std::vector<Animation> { getAnimationForShortcutIndex(1801), getAnimationForShortcutIndex(1803) }, 0.0);
std::vector<SyncedAnimation> gtaSyncedAnimations;

void initializeSyncedAnimations() {
	gtaSyncedAnimations = {
		SyncedAnimation("Manly handshake (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1801), getAnimationForShortcutIndex(1803)},-1),
		SyncedAnimation("Manly handshake (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1822), getAnimationForShortcutIndex(30710)},-1),
		SyncedAnimation("Manly handshake (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(30719), getAnimationForShortcutIndex(30720)},-1),
		SyncedAnimation("Bro hug (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(30697), getAnimationForShortcutIndex(30698)},-1),
		SyncedAnimation("Bro hug (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1812), getAnimationForShortcutIndex(1814)},-1),
		SyncedAnimation("Bro hug (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(30712), getAnimationForShortcutIndex(30713)},-1),
		SyncedAnimation("Gamer", std::vector<Animation> {getAnimationForShortcutIndex(7785)}, std::vector<Animation> {getAnimationForShortcutIndex(7786),getAnimationForShortcutIndex(7787)},std::vector<GTAObject> {getGTAObjectFromObjName("prop_controller_01"),getGTAObjectFromObjName("prop_headset_01")},0.0),
		SyncedAnimation("Enter door", std::vector<Animation> {getAnimationForShortcutIndex(12066)}, std::vector<Animation> {getAnimationForShortcutIndex(12069)},std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fa_frontdoor")},0.0),
		SyncedAnimation("Cowering (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1793), getAnimationForShortcutIndex(30700)},-1),
		SyncedAnimation("Cowering (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1815), getAnimationForShortcutIndex(30705)},-1),
		SyncedAnimation("Cowering (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1833), getAnimationForShortcutIndex(30715)},-1),
		SyncedAnimation("Backslap (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1790), getAnimationForShortcutIndex(30696)},-1),
		SyncedAnimation("Backslap (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1810), getAnimationForShortcutIndex(30704)},-1),
		SyncedAnimation("Backslap (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1829), getAnimationForShortcutIndex(30711)},-1),
		SyncedAnimation("Fistbump (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(30701), getAnimationForShortcutIndex(30702)},-1),
		SyncedAnimation("Fistbump (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(30706), getAnimationForShortcutIndex(30707)},-1),
		SyncedAnimation("Fistbump (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(30716), getAnimationForShortcutIndex(30717)},-1),
		SyncedAnimation("High five (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1798), getAnimationForShortcutIndex(30703)},-1),
		SyncedAnimation("High five (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(30708), getAnimationForShortcutIndex(30709)},-1),
		SyncedAnimation("High five (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1839), getAnimationForShortcutIndex(30718)},-1),
		SyncedAnimation("Sarcastic (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1825), getAnimationForShortcutIndex(1827)},-1),
		SyncedAnimation("Sarcastic (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1805), getAnimationForShortcutIndex(1807)},-1),
		SyncedAnimation("Sarcastic (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1843), getAnimationForShortcutIndex(1845)},-1),
		SyncedAnimation("Cougar takedown back", std::vector<Animation> {getAnimationForShortcutIndex(3812), getAnimationForShortcutIndex(3820)},0),
		SyncedAnimation("Cougar takedown front", std::vector<Animation> {getAnimationForShortcutIndex(3814), getAnimationForShortcutIndex(3821)},0),
		SyncedAnimation("Retriever takedown back", std::vector<Animation> {getAnimationForShortcutIndex(43873), getAnimationForShortcutIndex(43872)},0),
		SyncedAnimation("Retriever takedown front", std::vector<Animation> {getAnimationForShortcutIndex(4426), getAnimationForShortcutIndex(43875)},0),
		SyncedAnimation("Motorcycle knockout passenger (Franklin)", std::vector<Animation> {getAnimationForShortcutIndex(5191), getAnimationForShortcutIndex(5192)},0),
		SyncedAnimation("Vehicle knockout passenger (Franklin)", std::vector<Animation> {getAnimationForShortcutIndex(5188), getAnimationForShortcutIndex(5189)},0),
		SyncedAnimation("Hatchet takedown front", std::vector<Animation> {getAnimationForShortcutIndex(5792), getAnimationForShortcutIndex(5795)},0),
		SyncedAnimation("Hatchet takedown rear #1", std::vector<Animation> {getAnimationForShortcutIndex(5793), getAnimationForShortcutIndex(5796)},0),
		SyncedAnimation("Hatchet takedown rear #2", std::vector<Animation> {getAnimationForShortcutIndex(5794), getAnimationForShortcutIndex(5797)},0),
		SyncedAnimation("Knife takedown failed rear", std::vector<Animation> {getAnimationForShortcutIndex(5823), getAnimationForShortcutIndex(5859)},0),
		SyncedAnimation("Knife takedown failed front", std::vector<Animation> {getAnimationForShortcutIndex(5824), getAnimationForShortcutIndex(5860)},0),
		SyncedAnimation("Knife takedown front", std::vector<Animation> {getAnimationForShortcutIndex(5825), getAnimationForShortcutIndex(5836)},0),
		SyncedAnimation("Knife takedown front", std::vector<Animation> {getAnimationForShortcutIndex(5826), getAnimationForShortcutIndex(5837)},0),
		SyncedAnimation("Knife stealth takedown", std::vector<Animation> {getAnimationForShortcutIndex(5827), getAnimationForShortcutIndex(5838)},0),
		SyncedAnimation("Melee failed takedown front", std::vector<Animation> {getAnimationForShortcutIndex(6057), getAnimationForShortcutIndex(6068)},0),
		SyncedAnimation("Melee failed takedown rear", std::vector<Animation> {getAnimationForShortcutIndex(6058), getAnimationForShortcutIndex(6069)},0),
		SyncedAnimation("Melee stealth kill unarmed", std::vector<Animation> {getAnimationForShortcutIndex(6059), getAnimationForShortcutIndex(6070)},0),
		SyncedAnimation("Melee takedown front elbow", std::vector<Animation> {getAnimationForShortcutIndex(6060), getAnimationForShortcutIndex(6071)},0),
		SyncedAnimation("Melee takedown front uppercut", std::vector<Animation> {getAnimationForShortcutIndex(6061), getAnimationForShortcutIndex(6072)},0),
		SyncedAnimation("Melee takedown rear lefthook", std::vector<Animation> {getAnimationForShortcutIndex(6062), getAnimationForShortcutIndex(6073)},0),
		SyncedAnimation("stealth_kill_unarmed_a", std::vector<Animation> {getAnimationForShortcutIndex(6116), getAnimationForShortcutIndex(6130)},0),
		SyncedAnimation("stealth_kill_unarmed_hook_r 2833", std::vector<Animation> {getAnimationForShortcutIndex(6117), getAnimationForShortcutIndex(6131)},0),
		SyncedAnimation("stealth_kill_unarmed_non_lethal_a 3466", std::vector<Animation> {getAnimationForShortcutIndex(6118), getAnimationForShortcutIndex(6132)},0),
		SyncedAnimation("stealth_kill_unarmed_non_lethal_b 2533", std::vector<Animation> {getAnimationForShortcutIndex(6170), getAnimationForShortcutIndex(6176)},0),
		SyncedAnimation("takedown_front_backslap 2333", std::vector<Animation> {getAnimationForShortcutIndex(6119), getAnimationForShortcutIndex(6133)},0),
		SyncedAnimation("takedown_front_cross_r 2066", std::vector<Animation> {getAnimationForShortcutIndex(6120), getAnimationForShortcutIndex(6134)},0),
		SyncedAnimation("takedown_front_elbow 3233", std::vector<Animation> {getAnimationForShortcutIndex(6121), getAnimationForShortcutIndex(6135)},0),
		SyncedAnimation("takedown_front_headbutt 2266", std::vector<Animation> {getAnimationForShortcutIndex(6122), getAnimationForShortcutIndex(6136)},0),
		SyncedAnimation("takedown_front_low_punch 3066", std::vector<Animation> {getAnimationForShortcutIndex(6123), getAnimationForShortcutIndex(6137)},0),
		SyncedAnimation("takedown_front_slap 2399", std::vector<Animation> {getAnimationForShortcutIndex(6124), getAnimationForShortcutIndex(6138)},0),
		SyncedAnimation("takedown_front_uppercut 2533", std::vector<Animation> {getAnimationForShortcutIndex(6125), getAnimationForShortcutIndex(6139)},0),
		SyncedAnimation("takedown_rear_lefthook 2266", std::vector<Animation> {getAnimationForShortcutIndex(6126), getAnimationForShortcutIndex(6140)},0),
		SyncedAnimation("psycho_front_takedown 4333", std::vector<Animation> {getAnimationForShortcutIndex(6143), getAnimationForShortcutIndex(67770)},0),
		SyncedAnimation("psycho_front_takedown_b 4000", std::vector<Animation> {getAnimationForShortcutIndex(6144), getAnimationForShortcutIndex(67771)},0),
		SyncedAnimation("psycho_front_takedown_c", std::vector<Animation> {getAnimationForShortcutIndex(6113), getAnimationForShortcutIndex(67772)},0),
		SyncedAnimation("psycho_rear_takedown 3366", std::vector<Animation> {getAnimationForShortcutIndex(6145), getAnimationForShortcutIndex(6147)},0),
		SyncedAnimation("psycho_rear_takedown_b 3500", std::vector<Animation> {getAnimationForShortcutIndex(6146), getAnimationForShortcutIndex(6148)},0),
		SyncedAnimation("Darts intro #1", std::vector<Animation> {getAnimationForShortcutIndex(6306), getAnimationForShortcutIndex(6308)},0.5),
		SyncedAnimation("Darts intro #2", std::vector<Animation> {getAnimationForShortcutIndex(6315), getAnimationForShortcutIndex(6317)},0.5),
		SyncedAnimation("Darts winner #1", std::vector<Animation> {getAnimationForShortcutIndex(6323), getAnimationForShortcutIndex(6324)},-1),
		SyncedAnimation("Darts winner #2", std::vector<Animation> {getAnimationForShortcutIndex(6326), getAnimationForShortcutIndex(6327)},-1),
		SyncedAnimation("Darts winner #3", std::vector<Animation> {getAnimationForShortcutIndex(6329), getAnimationForShortcutIndex(6330)},-1),
		SyncedAnimation("Drinking with barman #1", std::vector<Animation> {getAnimationForShortcutIndex(6331), getAnimationForShortcutIndex(6334)},0),
		SyncedAnimation("Drinking with barman #2", std::vector<Animation> {getAnimationForShortcutIndex(6332), getAnimationForShortcutIndex(6335)},0),
		SyncedAnimation("Drinking with barman #3", std::vector<Animation> {getAnimationForShortcutIndex(6333), getAnimationForShortcutIndex(6336)},0),
		SyncedAnimation("Sex low vehicle", std::vector<Animation> {getAnimationForShortcutIndex(6767), getAnimationForShortcutIndex(6766)},0),
		SyncedAnimation("Pimp demands money", std::vector<Animation> {getAnimationForShortcutIndex(6862), getAnimationForShortcutIndex(6864)},0),
		SyncedAnimation("Prostitutes argue", std::vector<Animation> {getAnimationForShortcutIndex(6865), getAnimationForShortcutIndex(6866)},0),
		SyncedAnimation("Drinking at strip club #1", std::vector<Animation> {getAnimationForShortcutIndex(6940), getAnimationForShortcutIndex(6937)},1),
		SyncedAnimation("Drinking at strip club #2", std::vector<Animation> {getAnimationForShortcutIndex(6955), getAnimationForShortcutIndex(6952)},1),
		SyncedAnimation("Drinking at strip club #3", std::vector<Animation> {getAnimationForShortcutIndex(6964), getAnimationForShortcutIndex(6961)},1),
		SyncedAnimation("Drinking at strip club #4", std::vector<Animation> {getAnimationForShortcutIndex(6974), getAnimationForShortcutIndex(6971)},1),
		SyncedAnimation("Argueing (Jimmy and Simeon) #1", std::vector<Animation> {getAnimationForShortcutIndex(7703), getAnimationForShortcutIndex(7704)},-1),
		SyncedAnimation("Argueing (Jimmy and Simeon) #2", std::vector<Animation> {getAnimationForShortcutIndex(7705), getAnimationForShortcutIndex(7706)},-1),
		SyncedAnimation("Franklin and Lamar walk to car", std::vector<Animation> {getAnimationForShortcutIndex(7726), getAnimationForShortcutIndex(7727)},-1)
	};
}

std::vector<SyncedAnimation> getAllSyncedAnimations() {
	return gtaSyncedAnimations;
}
