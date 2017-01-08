#include "SyncedAnimation.h"
#include <vector>
#include <map>
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

SyncedAnimation::SyncedAnimation(std::string title, std::string category, bool isProperSynced, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, std::vector<GTAObject>  syncObjects, float deltaZLocation)
{
	m_title = title;
	m_category = category;
	if (m_category.empty()) {
		m_category = "<missing category>";
	}
	m_isProperSynced = isProperSynced;
	m_actorAnimations = actorAnimations;
	m_objectAnimations = objectAnimations;
	m_deltaZLocation = deltaZLocation;
	m_syncObjects = syncObjects;
	m_isNull = false;
}

void SyncedAnimation::executeSyncedAnimation(bool silent, std::vector<Actor*> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop, bool useFirstActorRotation, float rotation)
{
	log_to_file("SyncedAnimation->executeSyncedAnimation " + toString());
	DWORD ticksStart = GetTickCount();
	m_doLooping = doLoop;


	if (syncActors.size() < m_actorAnimations.size()) {
		if (!silent) {
			set_status_text("Missing " + std::to_string(m_actorAnimations.size()- syncActors.size()) + " actor(s) but will still execute synched animation");
		}
	}



	log_to_file("m_objectAnimation ");
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
			if (GetTickCount() - ticksStart > 5000) {
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
			if (GetTickCount() - ticksStart > 5000) {
				set_status_text("Failed to load object animations");
				return;
			}
		}
	}

	//load objects
	for (auto &gtaObject : m_syncObjects) {
		if (gtaObject.objHash == -1) {
			gtaObject.objHash = GAMEPLAY::GET_HASH_KEY(gtaObject.objName);
		}

		STREAMING::REQUEST_MODEL(gtaObject.objHash);
		while (!STREAMING::HAS_MODEL_LOADED(gtaObject.objHash))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 5000) {
				log_to_file("Failed to load GTAObject name:" + std::string(gtaObject.objName) + " Hash:" + std::to_string(gtaObject.objHash));
				set_status_text("Failed to load objects");
				return;
			}
		}
	}

	//Setup scene for synced animation
	Vector3 sceneLocation = directLocation;

	
	if (useFirstActorLocation && syncActors.size() >= 1) {
		sceneLocation = ENTITY::GET_ENTITY_COORDS(syncActors.at(0)->getActorPed(), true);
	}
	else {
		//Vector3 actorLocation = ENTITY::GET_ENTITY_COORDS(syncActors.at(0)->getActorPed(), true);
		//log_to_file("Difference in location x:" + std::to_string(sceneLocation.x - actorLocation.x) + " y:" + std::to_string(sceneLocation.y - actorLocation.y) + " z:" + std::to_string(sceneLocation.z - actorLocation.z));
	}
	float m_currentRotation =  rotation;
	if (useFirstActorRotation  && syncActors.size() >= 1) {
		m_currentRotation = ENTITY::GET_ENTITY_ROTATION(syncActors.at(0)->getActorPed(), 2).z;
	}


	if (m_isProperSynced) {
		log_to_file("About to create scene startHeading=" + std::to_string(m_currentRotation));
		m_sceneId = PED::CREATE_SYNCHRONIZED_SCENE(sceneLocation.x, sceneLocation.y, sceneLocation.z + m_deltaZLocation, 0.0, 0.0, m_currentRotation, 2);
		PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, doLoop);

		log_to_file("About to add animations for actors");
		//Add the animations to the scene
		int actorIndex = 0;
		for (auto &animation : m_actorAnimations) {
			if (syncActors.size() > actorIndex) {
				log_to_file("Adding animation to " + std::to_string(syncActors.at(actorIndex)->getActorPed()) + " actor index:" + std::to_string(actorIndex));
				AI::TASK_SYNCHRONIZED_SCENE(syncActors.at(actorIndex)->getActorPed(), m_sceneId, animation.animLibrary, animation.animName, 1000.0, -4.0, 64, 0, 0x447a0000, 0);
				m_pedsInScene.push_back(syncActors.at(actorIndex)->getActorPed());

				m_pedsInSceneStartLocation.push_back(ENTITY::GET_ENTITY_COORDS(syncActors.at(actorIndex)->getActorPed(), true));
				m_pedsInSceneStartHeading.push_back(ENTITY::GET_ENTITY_HEADING(syncActors.at(actorIndex)->getActorPed()));

				actorIndex++;
			}
			else {
				log_to_file("Too few actors, so skipping animation " + animation.strShortcutIndex);
			}
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
			ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(m_syncObjects[objectIndex].objReference, m_sceneId, animation.animName, animation.animLibrary, 1000.0, -4.0, 0, 0x447a0000);
			objectIndex++;
		}

		log_to_file("About to execute the synchronized scene");
		//execute the scene
		PED::SET_SYNCHRONIZED_SCENE_PHASE(m_sceneId, 0.0);
	}
	else {//"fake" synchronized scene where we should not teleport actors, only play animations on
		m_ticksStarted = GetTickCount();
		int actorIndex = 0;
		for (auto &animation : m_actorAnimations) {
			if (syncActors.size() > actorIndex) {
				int duration = animation.duration;
				if (doLoop) {
					duration = -1;
				}
				AI::TASK_PLAY_ANIM(syncActors.at(actorIndex)->getActorPed(), animation.animLibrary, animation.animName, 8.0f, -8.0f, duration, getDefaultAnimationFlag().id, 8.0f, 0, 0, 0);

				m_pedsInScene.push_back(syncActors.at(actorIndex)->getActorPed());
				m_pedsInSceneStartLocation.push_back(ENTITY::GET_ENTITY_COORDS(syncActors.at(actorIndex)->getActorPed(), true));
				m_pedsInSceneStartHeading.push_back(ENTITY::GET_ENTITY_HEADING(syncActors.at(actorIndex)->getActorPed()));

				actorIndex++;
			}
			else {
				log_to_file("Too few actors, so skipping animation " + animation.strShortcutIndex);
			}
		}

	}
}

void SyncedAnimation::previewSyncedAnimation(std::vector<Actor*> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop, bool useFirstActorRotation, float rotation)
{
	executeSyncedAnimation(false,syncActors, useFirstActorLocation, directLocation, true, useFirstActorLocation, rotation);
	PED::SET_SYNCHRONIZED_SCENE_RATE(m_sceneId, 0.1f);
}

void SyncedAnimation::updateLocationOfScene(Vector3 location)
{
	if (m_sceneId != 0) {
		PED::SET_SYNCHRONIZED_SCENE_ORIGIN(m_sceneId, location.x, location.y, location.z, 0, 0, m_currentRotation, 2);
	}
}



bool SyncedAnimation::isCompleted()
{
	if (m_isProperSynced) {
		if (m_sceneId != 0) {
			if (PED::IS_SYNCHRONIZED_SCENE_LOOPED(m_sceneId)) {
				return false;
			}
			float sceneStatus = PED::GET_SYNCHRONIZED_SCENE_PHASE(m_sceneId);
			if (sceneStatus >= 1.0) {
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
	else {
		if (m_doLooping) {
			log_to_file("Is looping therefore not complete");
			return false;
		}
		int maxLength = 0; 
		for (auto &animation : m_actorAnimations) {
			if (animation.duration > maxLength) {
				maxLength = animation.duration;
			}
		}
		if (GetTickCount() - m_ticksStarted >= (DWORD) maxLength) {
			return true;
		}
		else {
			return false;
		}


	}

}

void SyncedAnimation::cleanupAfterExecution(bool doDeleteObjects, bool teleportActorsBackToStart)
{
	log_to_file("SyncedAnimation cleanupAfterExecution ");
	if (m_isProperSynced) {
		int i = 0;
		for (auto &ped : m_pedsInScene) {
			//no idea what 2+3 param is
			ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(ped, 0.0, 1);

			if (teleportActorsBackToStart) {
				AI::CLEAR_PED_TASKS(ped);
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped, m_pedsInSceneStartLocation[i].x, m_pedsInSceneStartLocation[i].y, m_pedsInSceneStartLocation[i].z, 0, 0, 1);
				ENTITY::SET_ENTITY_HEADING(ped, m_pedsInSceneStartHeading[i]);
			}
			i++;
		}
		for (auto &obj : m_objectsInScene) {
			//no idea what 2+3 param is
			ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(obj, 0.0, 1);
			if (doDeleteObjects) {
				OBJECT::DELETE_OBJECT(&obj);
			}
		}
		if (doDeleteObjects) {
			for (auto &gtaObject : m_syncObjects) {
				gtaObject.objReference = 0;
			}
		}


		log_to_file("SyncedAnimation trying to dispose scene ");
		//dispose scene
		PED::_0xCD9CC7E200A52A6F(m_sceneId);
		m_sceneId = 0;
	}
	else {
		int i = 0;
		for (auto &ped : m_pedsInScene) {
			//no idea what 2+3 param is
			AI::CLEAR_PED_TASKS(ped);

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
				OBJECT::DELETE_OBJECT(&obj);
			}
		}
	}

}


void SyncedAnimation::setLooping(bool doLoop)
{
	if (m_sceneId!=0 && m_isProperSynced) {
		log_to_file("Setting looping for scene to " + doLoop);
		PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, doLoop);
	}
}


bool SyncedAnimation::matchesFilter(std::string filterStr)
{
	std::string m_titleLC = std::string(m_title);
	std::transform(m_titleLC.begin(), m_titleLC.end(), m_titleLC.begin(), ::tolower);

	std::transform(filterStr.begin(), filterStr.end(), filterStr.begin(), ::tolower);

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
			if (m_titleLC.find(token) == std::string::npos) {
				return false;
			}
		}
		else if (currentFitlerOperator == FILTER_NOT) {
			if (m_titleLC.find(token) != std::string::npos) {
				return false;
			}
		}
		else {//FILTER_OR
			if (m_titleLC.find(token) != std::string::npos) {
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

bool SyncedAnimation::matchesCategory(std::string categoryStr)
{
	if (std::string(m_category).find(categoryStr) == std::string::npos || (categoryStr.empty() && m_category.empty())) {
		return false;
	}
	else {
		return true;
	}
}

bool SyncedAnimation::isNull()
{
	return m_isNull;
}

SyncedAnimation* SyncedAnimation::createCopy()
{
	//remove any references
	for (auto &gtaObject : m_syncObjects) {
		gtaObject.objReference = 0;
	}
	SyncedAnimation* syncedAnimCopy = new SyncedAnimation(m_title,m_category,m_isProperSynced, m_actorAnimations, m_objectAnimations,m_syncObjects, m_deltaZLocation);
	return syncedAnimCopy;
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

void SyncedAnimation::clearObjectReferences()
{
	for (auto &gtaObject : m_syncObjects) {
		gtaObject.objReference = 0;
	}
}

std::string SyncedAnimation::getCategory()
{
	return m_category;
}

std::string SyncedAnimation::getTitle()
{
	return m_title;
}

bool SyncedAnimation::isProperSynced()
{
	return m_isProperSynced;
}

int SyncedAnimation::getLength()
{
	int maxLength = 0;
	for (auto animation : m_actorAnimations) {
		if (animation.duration > maxLength) {
			maxLength = animation.duration;
		}
	}
	return maxLength;
	
}

int SyncedAnimation::getNrOfActors()
{
	return m_actorAnimations.size();
}

bool SyncedAnimation::isActive()
{
	if (m_sceneId == 0) {
		return false;
	} {
		return true;
	}
}

float SyncedAnimation::getDeltaZ()
{
	return m_deltaZLocation;
}

void SyncedAnimation::setDeltaZ(float deltaZLocation)
{
	m_deltaZLocation = deltaZLocation;
}

//Defined all SyncedAnimations identified so far

//SyncedAnimation aSyncedAnimation = SyncedAnimation("Test",std::vector<Animation> { getAnimationForShortcutIndex(1801), getAnimationForShortcutIndex(1803) }, 0.0);
std::vector<SyncedAnimation> gtaSyncedAnimations;

void initializeSyncedAnimations() {
	gtaSyncedAnimations = { 
		SyncedAnimation("Manly handshake (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1801),getAnimationForShortcutIndex(1803),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Manly handshake (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1822),getAnimationForShortcutIndex(30710),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Manly handshake (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30719),getAnimationForShortcutIndex(30720),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bro hug (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30697),getAnimationForShortcutIndex(30698),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bro hug (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1812),getAnimationForShortcutIndex(1814),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bro hug (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30712),getAnimationForShortcutIndex(30713),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cowering (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1793),getAnimationForShortcutIndex(30700),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cowering (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1815),getAnimationForShortcutIndex(30705),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cowering (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1833),getAnimationForShortcutIndex(30715),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Backslap (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1790),getAnimationForShortcutIndex(30696),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Backslap (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1810),getAnimationForShortcutIndex(30704),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Backslap (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1829),getAnimationForShortcutIndex(30711),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Fistbump (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30701),getAnimationForShortcutIndex(30702),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Fistbump (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30706),getAnimationForShortcutIndex(30707),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Fistbump (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30716),getAnimationForShortcutIndex(30717),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("High five (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1798),getAnimationForShortcutIndex(30703),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("High five (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(30708),getAnimationForShortcutIndex(30709),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("High five (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1839),getAnimationForShortcutIndex(30718),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sarcastic (female - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1825),getAnimationForShortcutIndex(1827),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sarcastic (female - female)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1805),getAnimationForShortcutIndex(1807),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sarcastic (male - male)", "Celebration", true, std::vector<Animation> {getAnimationForShortcutIndex(1843),getAnimationForShortcutIndex(1845),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cougar takedown back", "Animal", false, std::vector<Animation> {getAnimationForShortcutIndex(3812),getAnimationForShortcutIndex(3820),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Cougar takedown front", "Animal", false, std::vector<Animation> {getAnimationForShortcutIndex(3814),getAnimationForShortcutIndex(3821),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Retriever takedown back", "Animal", false, std::vector<Animation> {getAnimationForShortcutIndex(43873),getAnimationForShortcutIndex(43872),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Retriever takedown front", "Animal", false, std::vector<Animation> {getAnimationForShortcutIndex(4426),getAnimationForShortcutIndex(43875),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Motorcycle knockout passenger (Franklin)", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(5191),getAnimationForShortcutIndex(5192),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Vehicle knockout passenger (Franklin)", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(5188),getAnimationForShortcutIndex(5189),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hatchet takedown front", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5792),getAnimationForShortcutIndex(5795),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hatchet takedown rear #1", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5793),getAnimationForShortcutIndex(5796),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hatchet takedown rear #2", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5794),getAnimationForShortcutIndex(5797),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown failed rear", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5823),getAnimationForShortcutIndex(5859),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown failed front", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5824),getAnimationForShortcutIndex(5860),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown front", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5825),getAnimationForShortcutIndex(5836),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown front", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5826),getAnimationForShortcutIndex(5837),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife stealth takedown", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(5827),getAnimationForShortcutIndex(5838),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee failed takedown front", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6057),getAnimationForShortcutIndex(6068),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee failed takedown rear", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6058),getAnimationForShortcutIndex(6069),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee stealth kill unarmed", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6059),getAnimationForShortcutIndex(6070),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee takedown front elbow", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6060),getAnimationForShortcutIndex(6071),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee takedown front uppercut", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6061),getAnimationForShortcutIndex(6072),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee takedown rear lefthook", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6062),getAnimationForShortcutIndex(6073),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_a", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6116),getAnimationForShortcutIndex(6130),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_hook_r 2833", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6117),getAnimationForShortcutIndex(6131),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_non_lethal_a 3466", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6118),getAnimationForShortcutIndex(6132),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_non_lethal_b 2533", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6170),getAnimationForShortcutIndex(6176),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_backslap 2333", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6119),getAnimationForShortcutIndex(6133),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_cross_r 2066", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6120),getAnimationForShortcutIndex(6134),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_elbow 3233", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6121),getAnimationForShortcutIndex(6135),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_headbutt 2266", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6122),getAnimationForShortcutIndex(6136),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_low_punch 3066", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6123),getAnimationForShortcutIndex(6137),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_slap 2399", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6124),getAnimationForShortcutIndex(6138),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_uppercut 2533", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6125),getAnimationForShortcutIndex(6139),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_rear_lefthook 2266", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6126),getAnimationForShortcutIndex(6140),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_front_takedown 4333", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6143),getAnimationForShortcutIndex(67770),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_front_takedown_b 4000", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6144),getAnimationForShortcutIndex(67771),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_front_takedown_c", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6113),getAnimationForShortcutIndex(67772),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_rear_takedown 3366", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6145),getAnimationForShortcutIndex(6147),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_rear_takedown_b 3500", "Melee", false, std::vector<Animation> {getAnimationForShortcutIndex(6146),getAnimationForShortcutIndex(6148),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Darts intro #1", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(6306),getAnimationForShortcutIndex(6308),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0.5),
		SyncedAnimation("Darts intro #2", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(6315),getAnimationForShortcutIndex(6317),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0.5),
		SyncedAnimation("Darts winner #1", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(6323),getAnimationForShortcutIndex(6324),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Darts winner #2", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(6326),getAnimationForShortcutIndex(6327),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Darts winner #3", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(6329),getAnimationForShortcutIndex(6330),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Drinking with barman #1", "Drinking/drugs", false, std::vector<Animation> {getAnimationForShortcutIndex(6331),getAnimationForShortcutIndex(6334),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drinking with barman #2", "Drinking/drugs", false, std::vector<Animation> {getAnimationForShortcutIndex(6332),getAnimationForShortcutIndex(6335),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drinking with barman #3", "Drinking/drugs", false, std::vector<Animation> {getAnimationForShortcutIndex(6333),getAnimationForShortcutIndex(6336),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Sex low vehicle", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(6767),getAnimationForShortcutIndex(6766),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Pimp demands money", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(6864),getAnimationForShortcutIndex(6863),}, std::vector<Animation> {getAnimationForShortcutIndex(6862),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cash_pile_02"),}, 0),
		SyncedAnimation("Prostitutes argue", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(6865),getAnimationForShortcutIndex(6866),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drinking at strip club #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(6940),getAnimationForShortcutIndex(6937),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Drinking at strip club #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(6955),getAnimationForShortcutIndex(6952),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Drinking at strip club #3", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(6964),getAnimationForShortcutIndex(6961),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Drinking at strip club #4", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(6974),getAnimationForShortcutIndex(6971),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Argueing (Jimmy and Simeon) #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(7703),getAnimationForShortcutIndex(7704),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Argueing (Jimmy and Simeon) #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(7705),getAnimationForShortcutIndex(7706),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Franklin and Lamar walk to car", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(7726),getAnimationForShortcutIndex(7727),}, std::vector<Animation> {getAnimationForShortcutIndex(7724),getAnimationForShortcutIndex(7725),}, std::vector<GTAObject> {getGTAObjectFromObjName("ninef2"),getGTAObjectFromObjName("rapidgt2"),}, -1),
		SyncedAnimation("Gamer with controller and headset", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(7785),}, std::vector<Animation> {getAnimationForShortcutIndex(7786),getAnimationForShortcutIndex(7787),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_controller_01"),getGTAObjectFromObjName("prop_headset_01"),}, 0),
		SyncedAnimation("Enter door", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(12066),}, std::vector<Animation> {getAnimationForShortcutIndex(12069),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fa_frontdoor"),}, 0),
		SyncedAnimation("Lapdance exit (exit, touching)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7020),getAnimationForShortcutIndex(7018),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance exit (exit, dismiss)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7016),getAnimationForShortcutIndex(7015),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (start dancing)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7001),getAnimationForShortcutIndex(6999),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance sit-down (sit down pt1)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7006),getAnimationForShortcutIndex(7004),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance sit-down (sit down pt2)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7025),getAnimationForShortcutIndex(7023),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (dance1)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7029),getAnimationForShortcutIndex(7027),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (touch, dance2)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7033),getAnimationForShortcutIndex(7031),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (touch, dance3)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7038),getAnimationForShortcutIndex(7036),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (no touch, dance2)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7034),getAnimationForShortcutIndex(7031),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (no touch, dance3)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7039),getAnimationForShortcutIndex(7036),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (idle)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7042),getAnimationForShortcutIndex(7044),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g sit-down (pt1)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7057),getAnimationForShortcutIndex(7058),getAnimationForShortcutIndex(7060),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g sit-down (pt2)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7084),getAnimationForShortcutIndex(7085),getAnimationForShortcutIndex(7087),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (exit, dismiss)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7070),getAnimationForShortcutIndex(7071),getAnimationForShortcutIndex(7073),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (touch, dance1)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7091),getAnimationForShortcutIndex(7092),getAnimationForShortcutIndex(7094),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (touch, dance2)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7097),getAnimationForShortcutIndex(7098),getAnimationForShortcutIndex(7100),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (touch, dance3)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7103),getAnimationForShortcutIndex(7104),getAnimationForShortcutIndex(7106),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Remove parachute #1 (male)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13664),}, std::vector<Animation> {getAnimationForShortcutIndex(22477),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #2 (male)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(25112),}, std::vector<Animation> {getAnimationForShortcutIndex(1739),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #3 (male)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(25111),}, std::vector<Animation> {getAnimationForShortcutIndex(1740),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #4 (female)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(1731),}, std::vector<Animation> {getAnimationForShortcutIndex(1733),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #5 (female)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(22479),}, std::vector<Animation> {getAnimationForShortcutIndex(1734),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #6 (female)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(22478),}, std::vector<Animation> {getAnimationForShortcutIndex(1736),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Lapdance (Wade)", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(7161),getAnimationForShortcutIndex(7160),getAnimationForShortcutIndex(7159),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0.2),
		SyncedAnimation("Thrown out", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(7158),getAnimationForShortcutIndex(7155),getAnimationForShortcutIndex(7156),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.35),
		SyncedAnimation("Sign in", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(7620),getAnimationForShortcutIndex(7621),}, std::vector<Animation> {getAnimationForShortcutIndex(7619),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_amb_clipboard_01"),}, -0.58),
		SyncedAnimation("Dodge car (crowd 1)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(7707),getAnimationForShortcutIndex(7708),getAnimationForShortcutIndex(7709),getAnimationForShortcutIndex(7710),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dodge car (crowd 2)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(7711),getAnimationForShortcutIndex(7712),getAnimationForShortcutIndex(7713),getAnimationForShortcutIndex(7714),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dodge car (crowd 3)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(7715),getAnimationForShortcutIndex(7716),getAnimationForShortcutIndex(7717),getAnimationForShortcutIndex(7718),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dodge car (push)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(7719),getAnimationForShortcutIndex(7720),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.2),
		SyncedAnimation("Gun to driver head (intro)", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(7794),getAnimationForShortcutIndex(7795),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Gun to driver head (loop)", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(7801),getAnimationForShortcutIndex(7802),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Tennis coach (practice swing)", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(7806),getAnimationForShortcutIndex(7810),}, std::vector<Animation> {getAnimationForShortcutIndex(7807),getAnimationForShortcutIndex(7808),getAnimationForShortcutIndex(7809),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_tennis_rack_01"),getGTAObjectFromObjName("prop_tennis_rack_01b"),getGTAObjectFromObjName("p_tennis_bag_01_s"),}, 0.2),
		SyncedAnimation("Close vault door", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(7930),}, std::vector<Animation> {getAnimationForShortcutIndex(7931),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_fin_vaultdoor_s"),}, -0.2),
		SyncedAnimation("Take crate from van", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(7937),}, std::vector<Animation> {getAnimationForShortcutIndex(7939),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_ld_gold_chest"),}, 0),
		SyncedAnimation("Exit car with rifles", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(7995),getAnimationForShortcutIndex(7996),}, std::vector<Animation> {getAnimationForShortcutIndex(7994),}, std::vector<GTAObject> {getGTAObjectFromObjName("dubsta"),}, -1.1),
		SyncedAnimation("Gathered around laptop", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8013),getAnimationForShortcutIndex(8014),getAnimationForShortcutIndex(8017),}, std::vector<Animation> {getAnimationForShortcutIndex(8015),getAnimationForShortcutIndex(8016),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_laptop_02_s"),getGTAObjectFromObjName("p_laptop_02_s"),}, -0.4),
		SyncedAnimation("Get in passenger seat", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(8040),getAnimationForShortcutIndex(8043),}, std::vector<Animation> {getAnimationForShortcutIndex(8042),}, std::vector<GTAObject> {getGTAObjectFromObjName("asea"),}, -0.5),
		SyncedAnimation("Pull lever", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(8068),}, std::vector<Animation> {getAnimationForShortcutIndex(8069),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_rail_controller"),}, 0),
		SyncedAnimation("Chad Hug #1", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8105),getAnimationForShortcutIndex(8106),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Chad Hug #2", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8101),getAnimationForShortcutIndex(8102),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Chad Hug #3", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8103),getAnimationForShortcutIndex(8104),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Hang and throw over ledge", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8127),getAnimationForShortcutIndex(8128),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -8.3),
		SyncedAnimation("Blowjob, kneeling", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(8132),getAnimationForShortcutIndex(8130),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.9),
		SyncedAnimation("Sex, standing", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(8134),getAnimationForShortcutIndex(8133),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Throw grenade launcher", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(8238),getAnimationForShortcutIndex(8237),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Drunken fist fight", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8253),getAnimationForShortcutIndex(8258),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Car flips", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(8424),getAnimationForShortcutIndex(8427),}, std::vector<Animation> {getAnimationForShortcutIndex(8426),}, std::vector<GTAObject> {getGTAObjectFromObjName("dubsta"),}, -1),
		SyncedAnimation("Hold dog by collar", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(8434),getAnimationForShortcutIndex(8435),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.1),
		SyncedAnimation("Let go of dog collar", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(8433),getAnimationForShortcutIndex(8432),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.1),
		SyncedAnimation("Yacht fight 1 (1/3)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8533),getAnimationForShortcutIndex(8534),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -3),
		SyncedAnimation("Yacht fight 1 (2/3)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8535),getAnimationForShortcutIndex(8536),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -3),
		SyncedAnimation("Yacht fight 1 (3/3)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8537),getAnimationForShortcutIndex(8538),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -3),
		SyncedAnimation("Yacht fight 2 (1/3)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8541),getAnimationForShortcutIndex(8542),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 2 (2/3)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8544),getAnimationForShortcutIndex(8545),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 2 (3/3 failure)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8539),getAnimationForShortcutIndex(8540),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 2 (3/3 success)", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8547),getAnimationForShortcutIndex(8548),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Pull over ledge", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(8572),getAnimationForShortcutIndex(8573),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.6),
		SyncedAnimation("Seated, touching hands", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8609),getAnimationForShortcutIndex(8611),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Open door #3 (double doors)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(8618),}, std::vector<Animation> {getAnimationForShortcutIndex(8617),getAnimationForShortcutIndex(8616),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_mm_doorm_r"),getGTAObjectFromObjName("v_ilev_mm_doorm_l"),}, -1),
		SyncedAnimation("Jimmy drugs michael #1", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(8653),getAnimationForShortcutIndex(8651),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.3),
		SyncedAnimation("Jimmy drugs michael #2", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(8735),getAnimationForShortcutIndex(8734),}, std::vector<Animation> {getAnimationForShortcutIndex(8733),}, std::vector<GTAObject> {getGTAObjectFromObjName("tailgater"),}, -0.3),
		SyncedAnimation("Ear piercing base", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8748),getAnimationForShortcutIndex(8746),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Ear piercing #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8741),getAnimationForShortcutIndex(8740),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Ear piercing #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8744),getAnimationForShortcutIndex(8743),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Ear piercing #3", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8751),getAnimationForShortcutIndex(8750),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Enter car panicked", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(8817),}, std::vector<Animation> {getAnimationForShortcutIndex(8816),}, std::vector<GTAObject> {getGTAObjectFromObjName("asea"),}, -0.5),
		SyncedAnimation("Interrogate", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(8921),getAnimationForShortcutIndex(8923),}, std::vector<Animation> {getAnimationForShortcutIndex(8920),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_corp_offchair"),}, 0),
		SyncedAnimation("Group talking", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9086),getAnimationForShortcutIndex(9087),getAnimationForShortcutIndex(9088),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Torture, stab in chest", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9174),getAnimationForShortcutIndex(9177),}, std::vector<Animation> {getAnimationForShortcutIndex(9171),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (intro)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9198),getAnimationForShortcutIndex(9201),}, std::vector<Animation> {getAnimationForShortcutIndex(9200),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (idle)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9190),getAnimationForShortcutIndex(9193),}, std::vector<Animation> {getAnimationForShortcutIndex(9192),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (loop)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9206),getAnimationForShortcutIndex(9209),}, std::vector<Animation> {getAnimationForShortcutIndex(9208),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (exit)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9226),getAnimationForShortcutIndex(9229),}, std::vector<Animation> {getAnimationForShortcutIndex(9228),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, waterboard (intro)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9269),getAnimationForShortcutIndex(9272),}, std::vector<Animation> {getAnimationForShortcutIndex(9265),getAnimationForShortcutIndex(9268),getAnimationForShortcutIndex(9271),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("p_loose_rag_01_s"),}, -0.6),
		SyncedAnimation("Torture, waterboard (kick)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9279),getAnimationForShortcutIndex(9283),}, std::vector<Animation> {getAnimationForShortcutIndex(9275),getAnimationForShortcutIndex(9278),getAnimationForShortcutIndex(9282),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("p_loose_rag_01_s"),}, -0.6),
		SyncedAnimation("Torture, waterboard (loop)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9298),getAnimationForShortcutIndex(9301),}, std::vector<Animation> {getAnimationForShortcutIndex(9294),getAnimationForShortcutIndex(9297),getAnimationForShortcutIndex(9300),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("prop_face_rag_01"),}, -0.6),
		SyncedAnimation("Torture, waterboard (outro)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9313),getAnimationForShortcutIndex(9317),}, std::vector<Animation> {getAnimationForShortcutIndex(9306),getAnimationForShortcutIndex(9312),getAnimationForShortcutIndex(9315),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("prop_face_rag_01"),}, -0.6),
		SyncedAnimation("Rollercoaster enter", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(2415),getAnimationForShortcutIndex(2416),}, std::vector<Animation> {getAnimationForShortcutIndex(2417),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Rollercoaster exit", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(2418),getAnimationForShortcutIndex(2419),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.9),
		SyncedAnimation("Rollercoaster hands-up start", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(2420),getAnimationForShortcutIndex(2421),}, std::vector<Animation> {getAnimationForShortcutIndex(2422),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Rollercoaster hands-up idle", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(2426),getAnimationForShortcutIndex(30746),}, std::vector<Animation> {getAnimationForShortcutIndex(2427),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Rollercoaster hands-up end", "Sports", true, std::vector<Animation> {getAnimationForShortcutIndex(2423),getAnimationForShortcutIndex(2424),}, std::vector<Animation> {getAnimationForShortcutIndex(2425),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Drink beer at safehouse #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2446),}, std::vector<Animation> {getAnimationForShortcutIndex(2447),getAnimationForShortcutIndex(2448),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2449),}, std::vector<Animation> {getAnimationForShortcutIndex(2450),getAnimationForShortcutIndex(2451),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #3", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(22905),}, std::vector<Animation> {getAnimationForShortcutIndex(2454),getAnimationForShortcutIndex(2453),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #4", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2454),}, std::vector<Animation> {getAnimationForShortcutIndex(2455),getAnimationForShortcutIndex(2456),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Smoke bong at safehouse #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2457),}, std::vector<Animation> {getAnimationForShortcutIndex(22904),getAnimationForShortcutIndex(2458),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -1),
		SyncedAnimation("Smoke bong beer at safehouse #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2459),}, std::vector<Animation> {getAnimationForShortcutIndex(22903),getAnimationForShortcutIndex(22902),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("v_res_tt_lighter"),}, -1),
		SyncedAnimation("Smoke bong beer at safehouse #3", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2460),}, std::vector<Animation> {getAnimationForShortcutIndex(22898),getAnimationForShortcutIndex(22900),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("ex_prop_exec_lighter_01"),}, -1),
		SyncedAnimation("Smoke bong beer at safehouse #4", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2461),}, std::vector<Animation> {getAnimationForShortcutIndex(22899),getAnimationForShortcutIndex(22901),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("lux_prop_lighter_luxe"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(22896),}, std::vector<Animation> {getAnimationForShortcutIndex(2465),getAnimationForShortcutIndex(2464),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2467),}, std::vector<Animation> {getAnimationForShortcutIndex(2468),getAnimationForShortcutIndex(2469),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #3", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2470),}, std::vector<Animation> {getAnimationForShortcutIndex(2471),getAnimationForShortcutIndex(2472),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #4", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2473),}, std::vector<Animation> {getAnimationForShortcutIndex(22895),getAnimationForShortcutIndex(2474),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink wine at safehouse #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2477),}, std::vector<Animation> {getAnimationForShortcutIndex(2478),getAnimationForShortcutIndex(2480),getAnimationForShortcutIndex(2479),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_01"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wine at safehouse #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2481),}, std::vector<Animation> {getAnimationForShortcutIndex(22893),getAnimationForShortcutIndex(22894),getAnimationForShortcutIndex(2482),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_01"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wine at safehouse #3", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2483),}, std::vector<Animation> {getAnimationForShortcutIndex(2484),getAnimationForShortcutIndex(2486),getAnimationForShortcutIndex(2485),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_02"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wine at safehouse #4", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(2487),}, std::vector<Animation> {getAnimationForShortcutIndex(2488),getAnimationForShortcutIndex(2490),getAnimationForShortcutIndex(2489),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_02"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wheatgrass at safehouse", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(22897),}, std::vector<Animation> {getAnimationForShortcutIndex(2462),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_w_grass_gls_s"),}, -1),
		SyncedAnimation("Car knockout", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(5188),getAnimationForShortcutIndex(5189),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Motorcycle knockout", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(5191),getAnimationForShortcutIndex(5192),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arm wrestling winner #1", "Sports", false, std::vector<Animation> {getAnimationForShortcutIndex(6206),getAnimationForShortcutIndex(6208),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arm wrestling winner #2", "Sports", false, std::vector<Animation> {getAnimationForShortcutIndex(6210),getAnimationForShortcutIndex(6212),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("CPR", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10902),getAnimationForShortcutIndex(10903),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("CPR Cower", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10904),getAnimationForShortcutIndex(10905),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Miss switch in car", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(12031),getAnimationForShortcutIndex(12032),getAnimationForShortcutIndex(12030),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("mid_mission_inside_helicopter_lester ", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(12036),getAnimationForShortcutIndex(12037),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("shop_ig_4_customer", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(12042),getAnimationForShortcutIndex(12045),}, std::vector<Animation> {getAnimationForShortcutIndex(12043),getAnimationForShortcutIndex(12044),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_ta_door"),getGTAObjectFromObjName("v_ilev_ta_tatgun"),}, -2),
		SyncedAnimation("artist_finishes_up_his_tattoo_artist", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(12055),getAnimationForShortcutIndex(1253),}, std::vector<Animation> {getAnimationForShortcutIndex(12054),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_ta_door"),}, -2),
		SyncedAnimation("Enter shop", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(12058),getAnimationForShortcutIndex(12059),}, std::vector<Animation> {getAnimationForShortcutIndex(12060),getAnimationForShortcutIndex(12057),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_ta_door"),getGTAObjectFromObjName("v_ilev_ta_door"),}, -1),
		SyncedAnimation("Tattoo #2", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(14856),getAnimationForShortcutIndex(14857),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.9),
		SyncedAnimation("Enter home #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(12062),}, std::vector<Animation> {getAnimationForShortcutIndex(12064),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter home #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(12066),}, std::vector<Animation> {getAnimationForShortcutIndex(12069),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("threaten_ortega_trev", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12107),getAnimationForShortcutIndex(12106),getAnimationForShortcutIndex(12105),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("dialogue_b_ron", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(12157),getAnimationForShortcutIndex(12156),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("jump_out_of_van_ds", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(12169),getAnimationForShortcutIndex(12170),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.1),
		SyncedAnimation("bikers_from_van_ped_a", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(12171),getAnimationForShortcutIndex(12172),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.1),
		SyncedAnimation("bikers_from_van_peda", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(12173),getAnimationForShortcutIndex(12174),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.1),
		SyncedAnimation("call_biker_biker", "Friendly interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(12197),getAnimationForShortcutIndex(12198),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("misstrevor3 bike_chat_a_1 intro", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12236),getAnimationForShortcutIndex(12237),}, std::vector<Animation> {getAnimationForShortcutIndex(12238),getAnimationForShortcutIndex(12239),}, std::vector<GTAObject> {getGTAObjectFromObjName("daemon"),getGTAObjectFromObjName("daemon"),}, -1),
		SyncedAnimation("misstrevor3 bike_chat_b_loop_1 loop", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12240),getAnimationForShortcutIndex(12241),}, std::vector<Animation> {getAnimationForShortcutIndex(12242),getAnimationForShortcutIndex(12243),}, std::vector<GTAObject> {getGTAObjectFromObjName("daemon"),getGTAObjectFromObjName("daemon"),}, -1),
		SyncedAnimation("misstrevor3 bike_chat_b_outro_1 outro", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12245),getAnimationForShortcutIndex(12246),}, std::vector<Animation> {getAnimationForShortcutIndex(12247),getAnimationForShortcutIndex(12248),}, std::vector<GTAObject> {getGTAObjectFromObjName("daemon"),getGTAObjectFromObjName("daemon"),}, -1),
		SyncedAnimation("Beatup dockworker intro", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12267),getAnimationForShortcutIndex(12268),getAnimationForShortcutIndex(12269),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Beatup dockworker main", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12264),getAnimationForShortcutIndex(12265),getAnimationForShortcutIndex(12266),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Beatup dockworker exit", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12258),getAnimationForShortcutIndex(12259),getAnimationForShortcutIndex(12260),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Beatup dockworker idle", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12261),getAnimationForShortcutIndex(12262),getAnimationForShortcutIndex(12263),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Shove", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(12270),getAnimationForShortcutIndex(12271),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Dive clear", "Other", false, std::vector<Animation> {getAnimationForShortcutIndex(12281),getAnimationForShortcutIndex(12282),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Jump out of van", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(12289),getAnimationForShortcutIndex(12290),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.2),
		SyncedAnimation("Store hold up cash", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13107),}, std::vector<Animation> {getAnimationForShortcutIndex(13108),getAnimationForShortcutIndex(13109),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_poly_bag_01_s"),getGTAObjectFromObjName("p_till_01_s"),}, 0),
		SyncedAnimation("Purchase beer", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13111),}, std::vector<Animation> {getAnimationForShortcutIndex(13110),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_beer_pissh"),}, 0),
		SyncedAnimation("Purchase beerbox", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13113),}, std::vector<Animation> {getAnimationForShortcutIndex(13112),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ret_ml_beerjak2"),}, 0),
		SyncedAnimation("Purchase chocbar", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13115),}, std::vector<Animation> {getAnimationForShortcutIndex(13114),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_choc_ego"),}, 0),
		SyncedAnimation("Purchase cigarette", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13117),}, std::vector<Animation> {getAnimationForShortcutIndex(13116),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_fag_packet_01"),}, 0),
		SyncedAnimation("Lap dance", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(13124),getAnimationForShortcutIndex(13123),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Arrest back-left", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13158),getAnimationForShortcutIndex(13172),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest back-right", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13159),getAnimationForShortcutIndex(13173),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest forward-left", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13161),getAnimationForShortcutIndex(13174),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest forward-right", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13162),getAnimationForShortcutIndex(13175),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor back-left", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13180),getAnimationForShortcutIndex(13181),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor back-right", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13184),getAnimationForShortcutIndex(13185),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor front-left", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13188),getAnimationForShortcutIndex(13189),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor front-right", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13192),getAnimationForShortcutIndex(13193),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Car bomb install", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13235),}, std::vector<Animation> {getAnimationForShortcutIndex(13234),}, std::vector<GTAObject> {getGTAObjectFromObjName("w_ex_pe"),}, 0),
		SyncedAnimation("Buzz in friend", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13392),getAnimationForShortcutIndex(13393),}, std::vector<Animation> {getAnimationForShortcutIndex(13391),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter door left #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13411),}, std::vector<Animation> {getAnimationForShortcutIndex(13410),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter door left #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13414),}, std::vector<Animation> {getAnimationForShortcutIndex(13413),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter door right", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13418),}, std::vector<Animation> {getAnimationForShortcutIndex(13417),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Exit door left", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13422),}, std::vector<Animation> {getAnimationForShortcutIndex(13421),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Exit door right", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13426),}, std::vector<Animation> {getAnimationForShortcutIndex(13425),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Lamar's introduction to multiplayer female", "Friendly interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13528),getAnimationForShortcutIndex(13529),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.5),
		SyncedAnimation("Lamar's introduction to multiplayer male", "Friendly interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13530),getAnimationForShortcutIndex(13531),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.5),
		SyncedAnimation("Shoot out", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13633),getAnimationForShortcutIndex(13634),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Handshake", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13669),getAnimationForShortcutIndex(13670),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("High five", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13672),getAnimationForShortcutIndex(13673),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hugs", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13675),getAnimationForShortcutIndex(13676),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Kisses", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13678),getAnimationForShortcutIndex(13679),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Prison break", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(13750),getAnimationForShortcutIndex(13748),getAnimationForShortcutIndex(13747),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Lap dance safehouse", "Explicit", true, std::vector<Animation> {getAnimationForShortcutIndex(13764),getAnimationForShortcutIndex(13765),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Drink whiskey #1 shot", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(13952),}, std::vector<Animation> {getAnimationForShortcutIndex(13957),getAnimationForShortcutIndex(13959),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, 0),
		SyncedAnimation("Drink whiskey #2 shot", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(13968),}, std::vector<Animation> {getAnimationForShortcutIndex(13973),getAnimationForShortcutIndex(13975),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, 0),
		SyncedAnimation("Drink whiskey #3 shot", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(13980),}, std::vector<Animation> {getAnimationForShortcutIndex(13985),getAnimationForShortcutIndex(13987),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, 0),
		SyncedAnimation("Drink whiskey #4 shot", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(13960),}, std::vector<Animation> {getAnimationForShortcutIndex(13965),getAnimationForShortcutIndex(13967),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, 0),
		SyncedAnimation("Drink whiskey #5 shot", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(13944),}, std::vector<Animation> {getAnimationForShortcutIndex(13949),getAnimationForShortcutIndex(13951),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, 0),
		SyncedAnimation("Torture, wrench intro", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9485),getAnimationForShortcutIndex(9487),}, std::vector<Animation> {getAnimationForShortcutIndex(9482),getAnimationForShortcutIndex(9488),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_cs_wrench"),}, 0),
		SyncedAnimation("Torture, wrench #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9458),getAnimationForShortcutIndex(9460),}, std::vector<Animation> {getAnimationForShortcutIndex(9455),getAnimationForShortcutIndex(9462),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_cs_wrench"),}, -0.65),
		SyncedAnimation("Torture, wrench #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9467),getAnimationForShortcutIndex(9469),}, std::vector<Animation> {getAnimationForShortcutIndex(9464),getAnimationForShortcutIndex(9471),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_cs_wrench"),}, -0.65),
		SyncedAnimation("Torture, wrench #3", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9476),getAnimationForShortcutIndex(9478),}, std::vector<Animation> {getAnimationForShortcutIndex(9473),getAnimationForShortcutIndex(9480),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_cs_wrench"),}, -0.65),
		SyncedAnimation("Take off mask", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9508),}, std::vector<Animation> {getAnimationForShortcutIndex(9509),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_mask_ballistic"),}, 0),
		SyncedAnimation("Driver crying #1", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(9562),getAnimationForShortcutIndex(9560),}, std::vector<Animation> {getAnimationForShortcutIndex(9558),}, std::vector<GTAObject> {getGTAObjectFromObjName("bodhi"),}, 0),
		SyncedAnimation("Driver crying #2", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(9563),getAnimationForShortcutIndex(9561),}, std::vector<Animation> {getAnimationForShortcutIndex(9559),}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (humane elevator #1)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9564),getAnimationForShortcutIndex(9569),getAnimationForShortcutIndex(9565),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Holdup (humane elevator #2)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9570),getAnimationForShortcutIndex(9567),getAnimationForShortcutIndex(9571),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Move through labs #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9575),getAnimationForShortcutIndex(9576),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Scientists working base", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(9583),getAnimationForShortcutIndex(9584),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.8),
		SyncedAnimation("Scientists working #1", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(9579),getAnimationForShortcutIndex(9580),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.8),
		SyncedAnimation("Scientists working #2", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(9581),getAnimationForShortcutIndex(9582),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.8),
		SyncedAnimation("Scientists working #3", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(9585),getAnimationForShortcutIndex(9586),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.8),
		SyncedAnimation("Holdup (scientists working #1)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9587),getAnimationForShortcutIndex(9590),getAnimationForShortcutIndex(9591),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Driver sad, push passenger away", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(9603),getAnimationForShortcutIndex(9602),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Driver tells passenger to stop", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(9605),getAnimationForShortcutIndex(9604),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Move through labs #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9606),getAnimationForShortcutIndex(9607),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Holdup (use keycard)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9608),getAnimationForShortcutIndex(9610),getAnimationForShortcutIndex(9609),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Holdup (scientist walking)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9622),getAnimationForShortcutIndex(9621),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Holdup (press button)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9625),getAnimationForShortcutIndex(9627),getAnimationForShortcutIndex(9626),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Take chemical", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9632),}, std::vector<Animation> {getAnimationForShortcutIndex(9634),getAnimationForShortcutIndex(9635),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_vial_01"),getGTAObjectFromObjName("p_chem_vial_02b_s"),}, -0.4),
		SyncedAnimation("Insert chemical", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9667),}, std::vector<Animation> {getAnimationForShortcutIndex(9668),getAnimationForShortcutIndex(9669),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_vial_01"),getGTAObjectFromObjName("p_chem_vial_02b_s"),}, -0.4),
		SyncedAnimation("Double doors (two actors)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(9683),getAnimationForShortcutIndex(9684),}, std::vector<Animation> {getAnimationForShortcutIndex(9685),getAnimationForShortcutIndex(9686),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_bl_door_l"),getGTAObjectFromObjName("v_ilev_bl_door_r"),}, 1.3),
		SyncedAnimation("Door kick #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11026),}, std::vector<Animation> {getAnimationForShortcutIndex(11029),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fa_frontdoor"),}, -1),
		SyncedAnimation("Mopping (wash mop)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9716),}, std::vector<Animation> {getAnimationForShortcutIndex(9715),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_mop_s"),getGTAObjectFromObjName("prop_cs_mopbucket_01"),}, -1),
		SyncedAnimation("Mopping (scrub)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9731),}, std::vector<Animation> {getAnimationForShortcutIndex(9732),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_mop_s"),getGTAObjectFromObjName("prop_cs_mopbucket_01"),}, -1),
		SyncedAnimation("Mopping (scrub small)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9735),}, std::vector<Animation> {getAnimationForShortcutIndex(9734),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_mop_s"),getGTAObjectFromObjName("prop_cs_mopbucket_01"),}, -1),
		SyncedAnimation("Show ID", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(9740),getAnimationForShortcutIndex(9739),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Michael's death (hanging)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9824),getAnimationForShortcutIndex(9826),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.2),
		SyncedAnimation("Michael's death (dropped)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9820),getAnimationForShortcutIndex(9821),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.2),
		SyncedAnimation("Michael's death (headbutt)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(9822),getAnimationForShortcutIndex(9823),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.2),
		SyncedAnimation("Devin hide in box #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9837),}, std::vector<Animation> {getAnimationForShortcutIndex(9836),getAnimationForShortcutIndex(9838),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_devin_box_01_s"),getGTAObjectFromObjName("prop_cs_tablet"),}, 0),
		SyncedAnimation("Devin hide in box #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9840),}, std::vector<Animation> {getAnimationForShortcutIndex(9839),getAnimationForShortcutIndex(9841),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_devin_box_01_s"),getGTAObjectFromObjName("prop_cs_tablet"),}, 0),
		SyncedAnimation("Push car", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(9846),getAnimationForShortcutIndex(9847),getAnimationForShortcutIndex(9848),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.5),
		SyncedAnimation("Dogs humping enter", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(9961),getAnimationForShortcutIndex(9962),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Dogs humping loop", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(9969),getAnimationForShortcutIndex(9970),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Dogs humping exit #1", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(9963),getAnimationForShortcutIndex(9964),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Dogs humping exit #2", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(9966),getAnimationForShortcutIndex(9968),getAnimationForShortcutIndex(9967),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Get in back of van with dog", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(9988),getAnimationForShortcutIndex(9987),}, std::vector<Animation> {getAnimationForShortcutIndex(9989),}, std::vector<GTAObject> {getGTAObjectFromObjName("speedo"),}, -0.2),
		SyncedAnimation("Wheelchair - turn right", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9889),}, std::vector<Animation> {getAnimationForShortcutIndex(9890),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wheelchair_01_s"),}, -0.35),
		SyncedAnimation("Wheelchair - stationary", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9893),}, std::vector<Animation> {getAnimationForShortcutIndex(9894),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wheelchair_01_s"),}, -0.35),
		SyncedAnimation("Wheelchair - turn left", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9895),}, std::vector<Animation> {getAnimationForShortcutIndex(9896),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wheelchair_01_s"),}, -0.35),
		SyncedAnimation("Wheelchair - use computer", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9897),}, std::vector<Animation> {getAnimationForShortcutIndex(9898),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wheelchair_01_s"),}, -0.35),
		SyncedAnimation("Throw passengers phone", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(9915),getAnimationForShortcutIndex(9916),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Get out of van (injured)", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(9910),}, std::vector<Animation> {getAnimationForShortcutIndex(9912),}, std::vector<GTAObject> {getGTAObjectFromObjName("speedo"),}, -0.2),
		SyncedAnimation("Get out of van with dog", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(9924),getAnimationForShortcutIndex(9923),getAnimationForShortcutIndex(9925),}, std::vector<Animation> {getAnimationForShortcutIndex(9926),}, std::vector<GTAObject> {getGTAObjectFromObjName("speedo"),}, -0.2),
		SyncedAnimation("Open door (strip club)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(9997),}, std::vector<Animation> {getAnimationForShortcutIndex(9999),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_strip_door_01"),}, 0.2),
		SyncedAnimation("Hairdressers leave", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10028),getAnimationForShortcutIndex(10041),}, std::vector<Animation> {getAnimationForShortcutIndex(10031),getAnimationForShortcutIndex(10056),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_bs_door"),getGTAObjectFromObjName("v_serv_bs_scissors"),}, -1),
		SyncedAnimation("Hairdressers sitdown", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10044),getAnimationForShortcutIndex(10037),}, std::vector<Animation> {getAnimationForShortcutIndex(10052),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_serv_bs_scissors"),}, -1),
		SyncedAnimation("Hairdressers haircut", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10086),getAnimationForShortcutIndex(10075),}, std::vector<Animation> {getAnimationForShortcutIndex(10096),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_serv_bs_scissors"),}, -1),
		SyncedAnimation("Throw helmet", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10160),getAnimationForShortcutIndex(10159),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.7),
		SyncedAnimation("Break door axe #1", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10190),getAnimationForShortcutIndex(10188),getAnimationForShortcutIndex(10189),}, std::vector<Animation> {getAnimationForShortcutIndex(10187),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_ld_fireaxe"),}, -1),
		SyncedAnimation("Break door axe #2", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10199),getAnimationForShortcutIndex(10198),getAnimationForShortcutIndex(10197),}, std::vector<Animation> {getAnimationForShortcutIndex(10196),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_ld_fireaxe"),}, -1),
		SyncedAnimation("Idle at computer #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10249),}, std::vector<Animation> {getAnimationForShortcutIndex(10244),getAnimationForShortcutIndex(10245),getAnimationForShortcutIndex(10248),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_clb_officechair_s"),getGTAObjectFromObjName("prop_cs_keyboard_01"),getGTAObjectFromObjName("prop_cs_mouse_01"),}, -1),
		SyncedAnimation("Idle at computer #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10261),}, std::vector<Animation> {getAnimationForShortcutIndex(10256),getAnimationForShortcutIndex(10257),getAnimationForShortcutIndex(10260),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_clb_officechair_s"),getGTAObjectFromObjName("prop_cs_keyboard_01"),getGTAObjectFromObjName("prop_cs_mouse_01"),}, -1),
		SyncedAnimation("Idle at computer #3", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10267),}, std::vector<Animation> {getAnimationForShortcutIndex(10262),getAnimationForShortcutIndex(10263),getAnimationForShortcutIndex(10266),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_clb_officechair_s"),getGTAObjectFromObjName("prop_cs_keyboard_01"),getGTAObjectFromObjName("prop_cs_mouse_01"),}, -1),
		SyncedAnimation("Pull to feet (alive)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10363),getAnimationForShortcutIndex(10361),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Pull to feet (dead)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10372),getAnimationForShortcutIndex(10370),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Rocking chair #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10526),}, std::vector<Animation> {getAnimationForShortcutIndex(10527),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_off_chair_01"),}, -0.6),
		SyncedAnimation("Rocking chair #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10528),}, std::vector<Animation> {getAnimationForShortcutIndex(10529),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_off_chair_01"),}, -0.6),
		SyncedAnimation("Rocking chair #3", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10530),}, std::vector<Animation> {getAnimationForShortcutIndex(10531),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_off_chair_01"),}, -0.6),
		SyncedAnimation("Rocking chair #4", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10532),}, std::vector<Animation> {getAnimationForShortcutIndex(10533),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_off_chair_01"),}, -0.6),
		SyncedAnimation("Stab guard", "Advanced Melee", true, std::vector<Animation> {getAnimationForShortcutIndex(10563),getAnimationForShortcutIndex(10562),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Enterboat action", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10573),}, std::vector<Animation> {getAnimationForShortcutIndex(10575),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_po1_01_doorm_s"),}, 0),
		SyncedAnimation("Enterboat stealth", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10576),}, std::vector<Animation> {getAnimationForShortcutIndex(10578),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_po1_01_doorm_s"),}, 0),
		SyncedAnimation("Back massage", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10583),getAnimationForShortcutIndex(10582),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Leg massage", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10609),getAnimationForShortcutIndex(10608),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dock beatup #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10665),getAnimationForShortcutIndex(10666),getAnimationForShortcutIndex(10667),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dock beatup #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10672),getAnimationForShortcutIndex(10673),getAnimationForShortcutIndex(10674),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Grab money, duffel (enter)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(1461),}, std::vector<Animation> {getAnimationForShortcutIndex(25701),getAnimationForShortcutIndex(25700),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_heist_bag_s_1"),getGTAObjectFromObjName("p_csh_strap_01_s"),}, -1),
		SyncedAnimation("Grab money, duffel (loop)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(1463),}, std::vector<Animation> {getAnimationForShortcutIndex(1464),getAnimationForShortcutIndex(1467),getAnimationForShortcutIndex(1465),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_heist_bag_s_1"),getGTAObjectFromObjName("p_csh_strap_01_s"),getGTAObjectFromObjName("hei_prop_heist_cash_pile"),}, -1),
		SyncedAnimation("Grab money, duffel (exit)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(25702),}, std::vector<Animation> {getAnimationForShortcutIndex(25699),getAnimationForShortcutIndex(1462),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_heist_bag_s_1"),getGTAObjectFromObjName("p_csh_strap_01_s"),}, -1),
		SyncedAnimation("Wheelchair idle", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11397),}, std::vector<Animation> {getAnimationForShortcutIndex(11398),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wheelchair_01_s"),}, -0.35),
		SyncedAnimation("Push up stairs", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10699),getAnimationForShortcutIndex(10700),getAnimationForShortcutIndex(10698),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.2),
		SyncedAnimation("Box falls", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10734),getAnimationForShortcutIndex(10733),getAnimationForShortcutIndex(10732),getAnimationForShortcutIndex(10731),}, std::vector<Animation> {getAnimationForShortcutIndex(10730),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_package_01"),}, -1),
		SyncedAnimation("Chatting leaning on wall (enter)", "Friendly interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(10741),getAnimationForShortcutIndex(10742),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Chatting leaning on wall (base)", "Friendly interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(10739),getAnimationForShortcutIndex(10740),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Chatting leaning on wall (exit)", "Friendly interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(10743),getAnimationForShortcutIndex(10744),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Exit car with stick and argue #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10805),getAnimationForShortcutIndex(10806),}, std::vector<Animation> {getAnimationForShortcutIndex(10804),}, std::vector<GTAObject> {getGTAObjectFromObjName("asea"),}, -0.35),
		SyncedAnimation("Exit car with stick and argue #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(10802),getAnimationForShortcutIndex(10803),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.35),
		SyncedAnimation("Catch bag", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(10923),getAnimationForShortcutIndex(10926),}, std::vector<Animation> {getAnimationForShortcutIndex(10922),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_heist_bag_s_1"),}, -1),
		SyncedAnimation("Unlock door #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10941),}, std::vector<Animation> {getAnimationForShortcutIndex(10940),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_janitor_frontdoor"),}, -1),
		SyncedAnimation("Unlock door #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(10939),}, std::vector<Animation> {getAnimationForShortcutIndex(10938),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_janitor_frontdoor"),}, -1),
		SyncedAnimation("Exit tank", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(10956),getAnimationForShortcutIndex(10957),getAnimationForShortcutIndex(10958),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Heli crash", "Vehicle", true, std::vector<Animation> {}, std::vector<Animation> {getAnimationForShortcutIndex(10991),}, std::vector<GTAObject> {getGTAObjectFromObjName("maverick"),}, 0),
		SyncedAnimation("Exit van (bank heist)", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(10996),getAnimationForShortcutIndex(10993),}, std::vector<Animation> {getAnimationForShortcutIndex(10999),}, std::vector<GTAObject> {getGTAObjectFromObjName("burrito"),}, -0.2),
		SyncedAnimation("Ready to enter (bank heist)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11003),getAnimationForShortcutIndex(11000),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.9),
		SyncedAnimation("Blowtorch door intro (bank heist)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11016),}, std::vector<Animation> {getAnimationForShortcutIndex(11018),getAnimationForShortcutIndex(11015),getAnimationForShortcutIndex(11019),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_welding_mask_01_s"),getGTAObjectFromObjName("prop_tool_blowtorch"),getGTAObjectFromObjName("v_ilev_cbankvaulgate01"),}, -1),
		SyncedAnimation("Blowtorch door loop (bank heist)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11021),}, std::vector<Animation> {getAnimationForShortcutIndex(11023),getAnimationForShortcutIndex(11020),getAnimationForShortcutIndex(11024),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_welding_mask_01_s"),getGTAObjectFromObjName("prop_tool_blowtorch"),getGTAObjectFromObjName("v_ilev_cbankvaulgate01"),}, -1),
		SyncedAnimation("Blowtorch door exit (bank heist)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(11013),getAnimationForShortcutIndex(11026),}, std::vector<Animation> {getAnimationForShortcutIndex(11012),getAnimationForShortcutIndex(11025),getAnimationForShortcutIndex(11029),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_tool_blowtorch"),getGTAObjectFromObjName("prop_welding_mask_01_s"),getGTAObjectFromObjName("v_ilev_cbankvaulgate01"),}, -1),
		SyncedAnimation("Empty safe exit (bank heist)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(11041),getAnimationForShortcutIndex(11044),}, std::vector<Animation> {getAnimationForShortcutIndex(11042),getAnimationForShortcutIndex(11045),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_heist_bag_s_1"),getGTAObjectFromObjName("p_ld_heist_bag_s_1"),}, -1),
		SyncedAnimation("Walk in bank, gunmen (bank heist)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11112),getAnimationForShortcutIndex(11097),}, std::vector<Animation> {getAnimationForShortcutIndex(11094),getAnimationForShortcutIndex(11095),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_bank4door02"),getGTAObjectFromObjName("v_ilev_bank4door01"),}, -0.9),
		SyncedAnimation("Walk in bank, hostages (bank heist)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11096),getAnimationForShortcutIndex(11108),getAnimationForShortcutIndex(11109),getAnimationForShortcutIndex(11110),getAnimationForShortcutIndex(11111),}, std::vector<Animation> {getAnimationForShortcutIndex(11093),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_cbankcountdoor01"),}, -0.9),
		SyncedAnimation("Hide behind door (bank heist)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11116),}, std::vector<Animation> {getAnimationForShortcutIndex(11117),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_cf_officedoor"),}, 0),
		SyncedAnimation("Cops investigate bank #1", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11132),getAnimationForShortcutIndex(11129),getAnimationForShortcutIndex(11130),getAnimationForShortcutIndex(11131),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 2.6),
		SyncedAnimation("Cops investigate bank #2", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11140),getAnimationForShortcutIndex(11142),getAnimationForShortcutIndex(11143),getAnimationForShortcutIndex(11137),getAnimationForShortcutIndex(11138),getAnimationForShortcutIndex(11139),}, std::vector<Animation> {getAnimationForShortcutIndex(11141),}, std::vector<GTAObject> {getGTAObjectFromObjName("police"),}, 2.6),
		SyncedAnimation("Shoot target on floor", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11175),getAnimationForShortcutIndex(11174),getAnimationForShortcutIndex(11176),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Throw paperball", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11360),}, std::vector<Animation> {getAnimationForShortcutIndex(11361),getAnimationForShortcutIndex(11362),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_off_chair_01"),getGTAObjectFromObjName("prop_paper_ball"),}, 0),
		SyncedAnimation("Climb from plane wreckage", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(11521),getAnimationForShortcutIndex(11522),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.2),
		SyncedAnimation("Knife struggle (top wins)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11609),getAnimationForShortcutIndex(11610),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Knife struggle (tackle)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11614),getAnimationForShortcutIndex(11616),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Knife struggle (loop)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11617),getAnimationForShortcutIndex(11618),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Read newspaper", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11633),}, std::vector<Animation> {getAnimationForShortcutIndex(11633),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_newspaper_s"),}, -1),
		SyncedAnimation("Close newspaper", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11635),}, std::vector<Animation> {getAnimationForShortcutIndex(11636),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_newspaper_s"),}, -1),
		SyncedAnimation("Run from heli crash", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11649),getAnimationForShortcutIndex(11650),getAnimationForShortcutIndex(11648),}, std::vector<Animation> {getAnimationForShortcutIndex(11646),}, std::vector<GTAObject> {getGTAObjectFromObjName("buzzard"),}, 0),
		SyncedAnimation("Throw passport from car", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(11749),getAnimationForShortcutIndex(11750),}, std::vector<Animation> {getAnimationForShortcutIndex(11751),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_passport_01"),}, 0),
		SyncedAnimation("Yankton hostages loop (prologue)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11756),getAnimationForShortcutIndex(11761),getAnimationForShortcutIndex(11758),getAnimationForShortcutIndex(11759),getAnimationForShortcutIndex(11760),getAnimationForShortcutIndex(11757),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Yankton hostages enter (prologue)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11762),getAnimationForShortcutIndex(11772),getAnimationForShortcutIndex(11768),getAnimationForShortcutIndex(11770),getAnimationForShortcutIndex(11771),getAnimationForShortcutIndex(11766),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Push hostage (prologue)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11796),getAnimationForShortcutIndex(11797),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Shut door with rifle", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11800),}, std::vector<Animation> {getAnimationForShortcutIndex(11801),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_cd_door2"),}, -1),
		SyncedAnimation("Human shield loop", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11811),getAnimationForShortcutIndex(11813),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Human shield exit", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11817),getAnimationForShortcutIndex(11818),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Pick up bag with rifle", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11816),}, std::vector<Animation> {getAnimationForShortcutIndex(11815),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_heist_bag_s_1"),}, 0),
		SyncedAnimation("Remove balaclava", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(11838),}, std::vector<Animation> {getAnimationForShortcutIndex(11839),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_trevor_prologe_bally_s"),}, 0),
		SyncedAnimation("Kick down hostage", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11850),getAnimationForShortcutIndex(11849),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("IAA human shield intro", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11942),getAnimationForShortcutIndex(11943),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("IAA human shield crouched loop", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11939),getAnimationForShortcutIndex(11940),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("IAA human shield standing loop", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11938),getAnimationForShortcutIndex(11941),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Roughen up, grab shirt intro", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11955),getAnimationForShortcutIndex(11957),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.7),
		SyncedAnimation("Roughen up, grab shirt shake", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11958),getAnimationForShortcutIndex(11960),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.7),
		SyncedAnimation("Run out of car with film", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(11970),}, std::vector<Animation> {getAnimationForShortcutIndex(11969),}, std::vector<GTAObject> {getGTAObjectFromObjName("asea"),}, 0.25),
		SyncedAnimation("Tracey human shield intro", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11974),getAnimationForShortcutIndex(11975),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Tracey human shield idle 1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11983),getAnimationForShortcutIndex(11984),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Tracey human shield idle 2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11986),getAnimationForShortcutIndex(11987),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Tracey human shield idle 3", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11989),getAnimationForShortcutIndex(11990),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Tracey human shield killed", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(11980),getAnimationForShortcutIndex(11981),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Amanda human shield idle 1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12007),getAnimationForShortcutIndex(12006),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.6),
		SyncedAnimation("Amanda human shield idle 2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12009),getAnimationForShortcutIndex(12008),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.6),
		SyncedAnimation("Amanda human shield idle 3", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12011),getAnimationForShortcutIndex(12010),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.6),
		SyncedAnimation("Amanda human shield idle 3", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12013),getAnimationForShortcutIndex(12012),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.6),
		SyncedAnimation("Amanda human shield killed", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12003),getAnimationForShortcutIndex(12002),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.6),
		SyncedAnimation("Amanda human shield freed", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(12005),getAnimationForShortcutIndex(12004),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.6),
		SyncedAnimation("Crying over dead body", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(12089),getAnimationForShortcutIndex(12090),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Neck snap", "Aggressive interaction", false, std::vector<Animation> {getAnimationForShortcutIndex(13275),getAnimationForShortcutIndex(13276),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Pass package across counter", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13294),getAnimationForShortcutIndex(13295),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Pass pistol across counter", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13296),getAnimationForShortcutIndex(13297),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Pass rifle across counter", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(13298),getAnimationForShortcutIndex(13299),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("MP intro plane flying", "Vehicle", true, std::vector<Animation> {}, std::vector<Animation> {getAnimationForShortcutIndex(13514),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_mp_jet_01_s"),}, 0),
		SyncedAnimation("MP intro plane landing", "Vehicle", true, std::vector<Animation> {}, std::vector<Animation> {getAnimationForShortcutIndex(13516),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_mp_jet_01_s"),}, 0),
		SyncedAnimation("Mp intro plane look out window", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(13519),}, std::vector<Animation> {getAnimationForShortcutIndex(13518),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_mp_jet_01_s"),}, 0),
		SyncedAnimation("Pin photo to wall", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(13821),}, std::vector<Animation> {getAnimationForShortcutIndex(13822),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_planning_photo"),}, 0),
		SyncedAnimation("Payphone conversation #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(14122),}, std::vector<Animation> {getAnimationForShortcutIndex(14123),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_phonebox_01b_s"),}, -1),
		SyncedAnimation("Payphone conversation #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(14130),}, std::vector<Animation> {getAnimationForShortcutIndex(14131),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_phonebox_01b_s"),}, -1),
		SyncedAnimation("Payphone conversation #3", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(14153),}, std::vector<Animation> {getAnimationForShortcutIndex(14154),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_phonebox_01b_s"),}, -1),
		SyncedAnimation("Payphone conversation #4", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(14169),}, std::vector<Animation> {getAnimationForShortcutIndex(14170),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_phonebox_01b_s"),}, -1),
		SyncedAnimation("Punch passenger out of car", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(14182),getAnimationForShortcutIndex(14181),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.7),
		SyncedAnimation("Put on parachute (male)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(14231),}, std::vector<Animation> {getAnimationForShortcutIndex(14232),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, -1),
		SyncedAnimation("Enter back of campervan", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14248),}, std::vector<Animation> {getAnimationForShortcutIndex(14250),}, std::vector<GTAObject> {getGTAObjectFromObjName("journey"),}, -0.5),
		SyncedAnimation("Exit back of campervan", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14251),}, std::vector<Animation> {getAnimationForShortcutIndex(14253),}, std::vector<GTAObject> {getGTAObjectFromObjName("journey"),}, -0.5),
		SyncedAnimation("Stand off bikers", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14280),getAnimationForShortcutIndex(14281),getAnimationForShortcutIndex(14282),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.1),
		SyncedAnimation("Give money to driver (front)", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(14286),getAnimationForShortcutIndex(14288),}, std::vector<Animation> {getAnimationForShortcutIndex(14287),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_prop_heist_cash_pile"),}, 0),
		SyncedAnimation("Give money to driver (rear left)", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(14289),getAnimationForShortcutIndex(14291),}, std::vector<Animation> {getAnimationForShortcutIndex(14290),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_prop_heist_cash_pile"),}, 0),
		SyncedAnimation("Give money to driver (rear right)", "Vehicle", false, std::vector<Animation> {getAnimationForShortcutIndex(14292),getAnimationForShortcutIndex(14294),}, std::vector<Animation> {getAnimationForShortcutIndex(14293),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_prop_heist_cash_pile"),}, 0),
		SyncedAnimation("Return wallet (female) #1", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14382),getAnimationForShortcutIndex(14381),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Return wallet (female) #2", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14393),getAnimationForShortcutIndex(14392),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Return wallet (female) #3", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14389),getAnimationForShortcutIndex(14388),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Return wallet (male) #1", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14398),getAnimationForShortcutIndex(14397),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Return wallet (male) #2", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14402),getAnimationForShortcutIndex(14401),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Return wallet (male) #3", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14406),getAnimationForShortcutIndex(14405),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Untie person on floor", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14459),getAnimationForShortcutIndex(14458),}, std::vector<Animation> {getAnimationForShortcutIndex(14457),getAnimationForShortcutIndex(14460),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ld_cable_tie_01_s"),getGTAObjectFromObjName("p_ld_cable_tie_01_s"),}, -1),
		SyncedAnimation("Untie person standing", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14907),getAnimationForShortcutIndex(14906),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Return stolen peyote #1", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14468),getAnimationForShortcutIndex(14467),}, std::vector<Animation> {getAnimationForShortcutIndex(14466),}, std::vector<GTAObject> {getGTAObjectFromObjName("peyote"),}, -0.6),
		SyncedAnimation("Return stolen peyote #2", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14472),getAnimationForShortcutIndex(14471),}, std::vector<Animation> {getAnimationForShortcutIndex(14470),}, std::vector<GTAObject> {getGTAObjectFromObjName("peyote"),}, -0.6),
		SyncedAnimation("Bag struggle loop #1", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14500),getAnimationForShortcutIndex(14499),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bag struggle loop #2", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14504),getAnimationForShortcutIndex(14505),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bag struggle, thief takes bag", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14511),getAnimationForShortcutIndex(14512),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Open door, hide intro", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14517),}, std::vector<Animation> {getAnimationForShortcutIndex(14516),}, std::vector<GTAObject> {getGTAObjectFromObjName("picador"),}, -0.3),
		SyncedAnimation("Open door, hide loop", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14519),}, std::vector<Animation> {getAnimationForShortcutIndex(14518),}, std::vector<GTAObject> {getGTAObjectFromObjName("picador"),}, -0.3),
		SyncedAnimation("Open door, hide outro", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14521),}, std::vector<Animation> {getAnimationForShortcutIndex(14520),}, std::vector<GTAObject> {getGTAObjectFromObjName("picador"),}, -0.3),
		SyncedAnimation("Help injured to feet", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14535),getAnimationForShortcutIndex(14536),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Car sex, lying down intro", "Explicit", false, std::vector<Animation> {getAnimationForShortcutIndex(14574),getAnimationForShortcutIndex(14573),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Car sex, lying down loop", "Explicit", false, std::vector<Animation> {getAnimationForShortcutIndex(14576),getAnimationForShortcutIndex(14575),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Car sex, lying down exit", "Explicit", false, std::vector<Animation> {getAnimationForShortcutIndex(14578),getAnimationForShortcutIndex(14577),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Gang intimidation", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14609),getAnimationForShortcutIndex(14610),getAnimationForShortcutIndex(14611),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Put prisoner in police car, loop", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14633),getAnimationForShortcutIndex(14635),getAnimationForShortcutIndex(14634),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.3),
		SyncedAnimation("Put prisoner in police car, exit", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14630),getAnimationForShortcutIndex(14632),getAnimationForShortcutIndex(14631),}, std::vector<Animation> {getAnimationForShortcutIndex(14629),}, std::vector<GTAObject> {getGTAObjectFromObjName("granger"),}, -0.3),
		SyncedAnimation("Kidnap, drag to van", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14650),getAnimationForShortcutIndex(14646),getAnimationForShortcutIndex(14648),}, std::vector<Animation> {getAnimationForShortcutIndex(14640),}, std::vector<GTAObject> {getGTAObjectFromObjName("burrito"),}, -0.2),
		SyncedAnimation("Take dog collar", "Animal", true, std::vector<Animation> {getAnimationForShortcutIndex(14707),getAnimationForShortcutIndex(14705),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Get in stockade", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14783),}, std::vector<Animation> {getAnimationForShortcutIndex(14784),}, std::vector<GTAObject> {getGTAObjectFromObjName("stockade"),}, -0.4),
		SyncedAnimation("Put case in stockade (calm)", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14777),}, std::vector<Animation> {getAnimationForShortcutIndex(14787),}, std::vector<GTAObject> {getGTAObjectFromObjName("stockade"),}, -0.4),
		SyncedAnimation("Put case in stockade (panic)", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(14778),}, std::vector<Animation> {getAnimationForShortcutIndex(14788),}, std::vector<GTAObject> {getGTAObjectFromObjName("stockade"),}, -0.4),
		SyncedAnimation("Shop robbery, customer through door", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14826),getAnimationForShortcutIndex(14831),getAnimationForShortcutIndex(14827),getAnimationForShortcutIndex(14830),}, std::vector<Animation> {getAnimationForShortcutIndex(14828),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_poly_bag_01_s"),}, -1),
		SyncedAnimation("Examine car together", "Friendly interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(14859),getAnimationForShortcutIndex(14860),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.4),
		SyncedAnimation("Smoke blunt, light", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15082),}, std::vector<Animation> {getAnimationForShortcutIndex(15083),getAnimationForShortcutIndex(15087),getAnimationForShortcutIndex(15088),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ret_fh_ashtray"),getGTAObjectFromObjName("prop_sh_joint_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.7),
		SyncedAnimation("Smoke blunt, loop", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15098),}, std::vector<Animation> {getAnimationForShortcutIndex(15099),getAnimationForShortcutIndex(15103),getAnimationForShortcutIndex(15104),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ret_fh_ashtray"),getGTAObjectFromObjName("prop_sh_joint_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.7),
		SyncedAnimation("Smoke blunt, exit", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15090),}, std::vector<Animation> {getAnimationForShortcutIndex(15091),getAnimationForShortcutIndex(15095),getAnimationForShortcutIndex(15096),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ret_fh_ashtray"),getGTAObjectFromObjName("prop_sh_joint_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.7),
		SyncedAnimation("Smoke cigar, enter", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15326),}, std::vector<Animation> {getAnimationForShortcutIndex(15324),getAnimationForShortcutIndex(15325),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_cigar_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.1),
		SyncedAnimation("Smoke cigar, exit", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15333),}, std::vector<Animation> {getAnimationForShortcutIndex(15331),getAnimationForShortcutIndex(15332),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_cigar_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.1),
		SyncedAnimation("Smoke cigar, loop #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15339),}, std::vector<Animation> {getAnimationForShortcutIndex(15337),getAnimationForShortcutIndex(15338),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_cigar_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.1),
		SyncedAnimation("Smoke cigar, loop #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15346),}, std::vector<Animation> {getAnimationForShortcutIndex(15344),getAnimationForShortcutIndex(15345),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_cigar_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -0.1),
		SyncedAnimation("Sit on table, drink beer intro", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15481),}, std::vector<Animation> {getAnimationForShortcutIndex(15482),getAnimationForShortcutIndex(15484),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_beer_bot_02"),getGTAObjectFromObjName("prop_bottle_cap_01"),}, -0.2),
		SyncedAnimation("Sit on table, drink beer #1", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15456),}, std::vector<Animation> {getAnimationForShortcutIndex(15457),getAnimationForShortcutIndex(15459),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_beer_bot_02"),getGTAObjectFromObjName("prop_bottle_cap_01"),}, -0.2),
		SyncedAnimation("Sit on table, drink beer #2", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15461),}, std::vector<Animation> {getAnimationForShortcutIndex(15462),getAnimationForShortcutIndex(15464),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_beer_bot_02"),getGTAObjectFromObjName("prop_bottle_cap_01"),}, -0.2),
		SyncedAnimation("Sit on table, drink beer #3", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15466),}, std::vector<Animation> {getAnimationForShortcutIndex(15467),getAnimationForShortcutIndex(15468),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_beer_bot_02"),getGTAObjectFromObjName("prop_bottle_cap_01"),}, -0.2),
		SyncedAnimation("Sit on table, drink beer #4", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15471),}, std::vector<Animation> {getAnimationForShortcutIndex(15472),getAnimationForShortcutIndex(15474),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_beer_bot_02"),getGTAObjectFromObjName("prop_bottle_cap_01"),}, -0.2),
		SyncedAnimation("Sit on table, drink beer #5", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15476),}, std::vector<Animation> {getAnimationForShortcutIndex(15477),getAnimationForShortcutIndex(15479),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_beer_bot_02"),getGTAObjectFromObjName("prop_bottle_cap_01"),}, -0.2),
		SyncedAnimation("Raspberryjam #1", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(15590),}, std::vector<Animation> {getAnimationForShortcutIndex(15592),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_mr_raspberry_01_s"),}, 0.2),
		SyncedAnimation("Raspberryjam #2", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(15593),}, std::vector<Animation> {getAnimationForShortcutIndex(15595),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_mr_raspberry_01_s"),}, 0.2),
		SyncedAnimation("Raspberryjam #3", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(15596),}, std::vector<Animation> {getAnimationForShortcutIndex(15598),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_mr_raspberry_01_s"),}, 0.2),
		SyncedAnimation("Raspberryjam #4", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(15599),}, std::vector<Animation> {getAnimationForShortcutIndex(15601),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_mr_raspberry_01_s"),}, 0.2),
		SyncedAnimation("Huff gas and pass out", "Drinking/drugs", true, std::vector<Animation> {getAnimationForShortcutIndex(15603),}, std::vector<Animation> {getAnimationForShortcutIndex(15605),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_rolled_sock_02"),}, -0.1),
		SyncedAnimation("Weed depository (switch)", "Other", true, std::vector<Animation> {getAnimationForShortcutIndex(17089),getAnimationForShortcutIndex(17091),}, std::vector<Animation> {getAnimationForShortcutIndex(17092),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_weed_bottle_s"),}, -0.4),
		SyncedAnimation("Talking to taxi passenger (switch)", "Vehicle", true, std::vector<Animation> {getAnimationForShortcutIndex(17109),getAnimationForShortcutIndex(17110),}, std::vector<Animation> {getAnimationForShortcutIndex(17111),}, std::vector<GTAObject> {getGTAObjectFromObjName("taxi"),}, 0),
		SyncedAnimation("Gang taunt loop (switch)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(17154),getAnimationForShortcutIndex(17155),getAnimationForShortcutIndex(17156),getAnimationForShortcutIndex(17157),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Gang taunt exit (switch)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(17149),getAnimationForShortcutIndex(17150),getAnimationForShortcutIndex(17151),getAnimationForShortcutIndex(17152),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Stop gang fight loop (switch)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(17165),getAnimationForShortcutIndex(17168),getAnimationForShortcutIndex(17166),getAnimationForShortcutIndex(17167),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Stop gang fight exit (switch)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(17160),getAnimationForShortcutIndex(17163),getAnimationForShortcutIndex(17161),getAnimationForShortcutIndex(16162),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Shoved by gangster (switch)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(17198),getAnimationForShortcutIndex(17199),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Toss garbage bag (switch)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17211),}, std::vector<Animation> {getAnimationForShortcutIndex(17209),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_rub_binbag_01"),}, -1),
		SyncedAnimation("Toss cup (switch)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17225),}, std::vector<Animation> {getAnimationForShortcutIndex(17227),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ing_coffeecup_01"),}, 0),
		SyncedAnimation("Ironing clothes loop (switch)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17231),}, std::vector<Animation> {getAnimationForShortcutIndex(17233),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_ironing_board"),}, -0.55),
		SyncedAnimation("Ironing clothes exit (switch)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17234),}, std::vector<Animation> {getAnimationForShortcutIndex(17237),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_ironing_board"),}, -0.55),
		SyncedAnimation("Shut laptop, loop (switch)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17259),}, std::vector<Animation> {getAnimationForShortcutIndex(17261),getAnimationForShortcutIndex(17260),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_laptop_02"),getGTAObjectFromObjName("p_clb_officechair_s"),}, 0),
		SyncedAnimation("Shut laptop, exit (switch)", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17254),}, std::vector<Animation> {getAnimationForShortcutIndex(17258),getAnimationForShortcutIndex(17256),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_cs_laptop_02"),getGTAObjectFromObjName("p_clb_officechair_s"),}, 0),
		SyncedAnimation("Tanisha argument (switch)", "Aggressive interaction", true, std::vector<Animation> {getAnimationForShortcutIndex(17304),getAnimationForShortcutIndex(17305),}, std::vector<Animation> {getAnimationForShortcutIndex(17307),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fa_frontdoor"),}, 0.1),
		SyncedAnimation("Throw cup", "Individual w/props", true, std::vector<Animation> {getAnimationForShortcutIndex(17312),}, std::vector<Animation> {getAnimationForShortcutIndex(17314),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_ing_coffeecup_01"),}, 0),
 };
}

std::vector<SyncedAnimation> getAllSyncedAnimations() {
	return gtaSyncedAnimations;
}

std::vector<SyncedAnimation> getSyncedAnimations(std::string category) {
	std::vector<SyncedAnimation> syncedAnimations;
	for (auto syncedAnim : gtaSyncedAnimations) {
		if (syncedAnim.matchesCategory(category) ) {
			syncedAnimations.push_back(syncedAnim);
		}
	}

	//std::sort(syncedAnimations.begin(), syncedAnimations.end());
	return syncedAnimations;
}

std::vector<std::pair<std::string, int>>  getAllSyncedAnimationCategories()
{
	std::map <std::string,std::string> mapCategories;
	log_to_file("Copying categories to map");
	for (auto syncedAnim : gtaSyncedAnimations) {
		mapCategories.insert({ syncedAnim.getCategory(),syncedAnim.getCategory() });
	}
	log_to_file("Copying categories to vector");
	std::vector<std::pair<std::string, int>> vCategories;
	for (std::map <std::string, std::string>::iterator it = mapCategories.begin(); it != mapCategories.end(); ++it) {
		std::string category = it->first;
		
		//not the most efficient, but shouldn't be a perf issue
		int count = getSyncedAnimations(category).size();
		vCategories.push_back(std::make_pair(category, count));
	}
	log_to_file("Sorting categories");
	std::sort(vCategories.begin(), vCategories.end());


	return vCategories;
}
