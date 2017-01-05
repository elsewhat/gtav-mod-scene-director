#pragma once
#include <vector>
#include <string>
#include <windows.h>
#include "..\..\inc\types.h"
#include "utils.h"
#include "Animation.h"
class Actor;
#include "Actor.h"
struct GTAObject;
#include "GTAObject.h"



class SyncedAnimation
{
private:
	std::string m_title;
	std::string m_category;
	float m_deltaZLocation;
	bool m_isNull = true;
	bool m_doLooping = false;
	bool m_isProperSynced = true;
	float m_currentRotation = 0.0f;
	DWORD m_ticksStarted;
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
	SyncedAnimation(std::string title, std::string category, bool isProperSynced, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, std::vector<GTAObject>  syncObjects, float deltaZLocation);

	void executeSyncedAnimation(bool silent, std::vector<Actor*> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop, bool useFirstActorRotation, float rotation);
	void previewSyncedAnimation(std::vector<Actor*> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop, bool useFirstActorRotation, float rotation);
	void updateLocationOfScene(Vector3 location);
	bool isCompleted();
	void cleanupAfterExecution(bool deleteObjects, bool teleportActorsBackToStart);
	void setLooping(bool doLooping);


	bool matchesFilter(std::string filterStr);
	bool matchesCategory(std::string categoryStr);
	bool isNull();
	SyncedAnimation* createCopy();
	std::string toString();
	void clearObjectReferences();

	std::string getCategory();
	std::string getTitle();
	bool isProperSynced();

	int getLength();
	int getNrOfActors();

	bool isActive();

	float getDeltaZ();
	void setDeltaZ(float deltaZLocation);

};

//Get all the synced animation defined in the mod
std::vector<SyncedAnimation> getAllSyncedAnimations();
std::vector<SyncedAnimation> getSyncedAnimations(std::string category);
std::vector<std::pair<std::string,int>> getAllSyncedAnimationCategories();
void initializeSyncedAnimations();


//lazy workaround. Could have have made a good interface to both instance instead but will not prioritize refactoring now
struct AnimationTrigger {
	AnimationSequence animationSequence;
	SyncedAnimation syncedAnimation;
	bool isAnimSeq = false;
	bool isSyncAnim = false;

	void setAnimationSequence(AnimationSequence animationSeq) {
		animationSequence = animationSeq;
		isAnimSeq = true;
		isSyncAnim = false;
	}

	void setSyncedAnimation(SyncedAnimation syncedAnim) {
		syncedAnimation = syncedAnim;
		isAnimSeq = false;
		isSyncAnim = true;
	}

	bool isAnimationSequence() {
		return isAnimSeq;
	}

	bool isSyncedAnimation() {
		return isSyncAnim;
	}

	AnimationSequence getAnimationSequence() {
		return animationSequence;
	}

	SyncedAnimation getSyncedAnimation() {
		return syncedAnimation;
	}

	static AnimationTrigger getNullAnimationTrigger() {
		AnimationTrigger animTrigger;
		animTrigger.isAnimSeq = false;
		animTrigger.isSyncAnim = false;
		return animTrigger;
	}

	bool isNullAnimationTrigger() {
		if (!isAnimSeq && !isSyncAnim) {
			return true;
		}
		return false;
	}

};