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
