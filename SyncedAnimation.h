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
	std::vector<GTAObject>  m_syncObjects;

	std::vector<Ped>  m_pedsInScene;
	std::vector<Vector3>  m_pedsInSceneStartLocation;
	std::vector<float>  m_pedsInSceneStartHeading;
	std::vector<Object>  m_objectsInScene;
	int m_sceneId = 0;
public:
	SyncedAnimation();
	SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, float deltaZLocation);
	SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, std::vector<GTAObject>  syncObjects, float deltaZLocation);

	void executeSyncedAnimation(std::vector<Actor> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop);
	bool isCompleted();
	void cleanupAfterExecution(bool deleteObjects, bool teleportActorsBackToStart);
	void setLooping(bool doLooping);


	bool matchesFilter(std::string filterStr);
	bool isNull();
	std::string toString();
};

//Get all the synced animation defined in the mod
std::vector<SyncedAnimation> getAllSyncedAnimations();
void initializeSyncedAnimations();