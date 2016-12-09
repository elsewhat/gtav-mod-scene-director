#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include "..\..\inc\types.h"
#include "utils.h"
#include "Animation.h"
#include "Actor.h"
#include "GTAObject.h"


class SyncedAnimation
{
private:
	std::string m_title;
	float m_deltaZLocation;
	bool m_isNull = true;
	std::vector<Animation>  m_actorAnimations;
	std::vector<Animation>  m_objectAnimations;

	std::vector<Ped>  m_pedsInScene;
	std::vector<Object>  m_objectsInScene;
	int m_sceneId = 0;
public:
	SyncedAnimation();
	SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, float deltaZLocation);
	SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, float deltaZLocation);

	void executeSyncedAnimation(std::vector<Actor> syncActors, std::vector<GTAObject>  syncObjects, bool useFirstActorLocation, Vector3 directLocation);
	bool isCompleted();
	void cleanupAfterExecution();


	bool matchesFilter(std::string filterStr);
	bool isNull();
	std::string toString();
};

//Get all the synced animation defined in the mod
std::vector<SyncedAnimation> getAllSyncedAnimations();
void initializeSyncedAnimations();