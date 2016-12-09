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

SyncedAnimation::SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, float deltaZLocation)
{
	m_title = title;
	m_actorAnimations = actorAnimations;
	m_objectAnimations = objectAnimations;
	m_deltaZLocation = deltaZLocation;
	m_isNull = false;
}

void SyncedAnimation::executeSyncedAnimation(std::vector<Actor> syncActors, std::vector<GTAObject>  syncObjects, bool useFirstActorLocation, Vector3 directLocation)
{
	log_to_file("SyncedAnimation->executeSyncedAnimation");
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
		if (syncObjects.size() <= i || syncObjects[i].isNull()) {
			set_status_text("Must have at least " + std::to_string(m_actorAnimations.size()) + " objects");
			return;
		}
	}


	//load actor animations
	for (auto &animation : m_actorAnimations) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
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
	for (auto &gtaObject : syncObjects) {
		STREAMING::REQUEST_MODEL(gtaObject.objHash);
		while (!STREAMING::HAS_MODEL_LOADED(gtaObject.objHash))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
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
		actorIndex++;
	}

	log_to_file("About to add animations for objects");
	int objectIndex = 0;
	for (auto &animation : m_objectAnimations) {
		//create object if it doesn't exist
		if (syncObjects[objectIndex].objReference == 0) {
			log_to_file("About to create object with name " + std::string(syncObjects[objectIndex].objName) + " and hash " + std::to_string(syncObjects[objectIndex].objHash));
			int newObjectRef = OBJECT::CREATE_OBJECT(syncObjects[objectIndex].objHash, sceneLocation.x, sceneLocation.y, sceneLocation.z, true, true, false);
			syncObjects[objectIndex].objReference = newObjectRef;
			m_objectsInScene.push_back(newObjectRef);
		}
		ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(syncObjects[objectIndex].objReference, m_sceneId, animation.animLibrary, animation.animName, 1000.0, -4.0, 0,0x447a0000);
		objectIndex++;
	}

	log_to_file("About to execute the synchronized scene");
	//execute the scene
	PED::SET_SYNCHRONIZED_SCENE_PHASE(m_sceneId, 0.0);
}

bool SyncedAnimation::isCompleted()
{
	if (m_sceneId != 0) {
		if (PED::IS_SYNCHRONIZED_SCENE_RUNNING(m_sceneId)) {
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

void SyncedAnimation::cleanupAfterExecution()
{
	log_to_file("SyncedAnimation cleanupAfterExecution ");
	for (auto &ped : m_pedsInScene) {
		//no idea what 2+3 param is
		ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(ped, 0.0, 1);
	}
	for (auto &obj : m_objectsInScene) {
		//no idea what 2+3 param is
		ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(obj, 0.0, 1);
	}
	log_to_file("SyncedAnimation trying to dispose scene ");
	//dispose scene
	PED::_0xCD9CC7E200A52A6F(m_sceneId);
	m_sceneId = 0;

	//TODO: Delete objects?
}


bool SyncedAnimation::isNull()
{
	return m_isNull;
}

//Defined all SyncedAnimations identified so far

//SyncedAnimation aSyncedAnimation = SyncedAnimation("Test",std::vector<Animation> { getAnimationForShortcutIndex(1801), getAnimationForShortcutIndex(1803) }, 0.0);

std::vector<SyncedAnimation> gtaSyncedAnimations = {
	SyncedAnimation("Manly handshake(female - female)",std::vector<Animation> {getAnimationForShortcutIndex(1801), getAnimationForShortcutIndex(1803) }, 0.0),
	SyncedAnimation("Manly handshake (female - male)",std::vector<Animation> {getAnimationForShortcutIndex(1822), getAnimationForShortcutIndex(30710) }, 0.0),
	SyncedAnimation("Manly handshake (male - male)",std::vector<Animation> {getAnimationForShortcutIndex(30719), getAnimationForShortcutIndex(30720) }, 0.0),
	SyncedAnimation("Bro hug (female - female)",std::vector<Animation> {getAnimationForShortcutIndex(30697), getAnimationForShortcutIndex(30698) }, 0.0),
};

std::vector<SyncedAnimation> getSyncedAnimation() {
	return gtaSyncedAnimations;
}
