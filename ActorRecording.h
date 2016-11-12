#pragma once
#include <vector>
#include "..\..\inc\types.h"
#include "script.h"
#include "scenario.h"
#include "Animation.h"
#include <string>
#include <memory>

//forward declaration
class Actor;
struct Scenario;

class ActorRecordingItem {
protected:
	DWORD m_ticksAfterRecordStart;
	Ped m_actorPed;
	Vector3 m_location;
	DWORD m_ticksDeltaCheckCompletion;
	DWORD m_ticksLength;
	DWORD m_ticksDeltaWhenRecorded;
public:
	ActorRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actorPed, Vector3 location);

	DWORD getTicksAfterRecordStart();
	Vector3 getLocation();


	DWORD getTicksDeltaCheckCompletion();
	void setTicksDeltaCheckCompletion(DWORD ticksDeltaCheckCompletion);

	void setTicksLength(DWORD ticks);
	DWORD getTicksLength();

	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)=0;
	virtual bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem,DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)=0;
	virtual std::string toString();
	virtual void executeNativesAfterRecording(Actor actor);
	virtual void drawMarkerForRecording(bool isCurrent);
};

class ActorOnFootMovementRecordingItem : public ActorRecordingItem {
protected:
	float m_walkSpeed;
	float m_heading;
public:
	ActorOnFootMovementRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed, float headingAtEnd);
	float getWalkSpeed();
	float getHeading();
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	virtual bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location);
	std::string toString() override;

};

class ActorSpeakRecordingItem : public ActorOnFootMovementRecordingItem {
protected:
	bool m_isMovingWhileSpeaking;
public:
	ActorSpeakRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed, float headingAtEnd, bool isMovingWhileSpeaking);
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	virtual bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location);
	std::string toString() override;
	virtual void executeNativesAfterRecording(Actor actor) override;
};

class ActorJumpingRecordingItem : public ActorOnFootMovementRecordingItem {
protected:
public:
	ActorJumpingRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed, float headingAtEnd);
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	virtual bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location);
	std::string toString() override;

};

class ActorStandingStillRecordingItem : public ActorRecordingItem {
protected:
	float m_heading;
public:
	ActorStandingStillRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float heading);
	float getHeading();
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem,DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;

};

class ActorAimAtRecordingItem : public ActorRecordingItem {
protected:
	Entity m_aimedAtEntity;
public:
	ActorAimAtRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Entity aimedAtEntity);
	Entity getAimedAtEntity();
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
	virtual void executeNativesAfterRecording(Actor actor) override;
};

class ActorShootAtEntityRecordingItem : public ActorRecordingItem {
protected:
	Entity m_shotAtEntity;
public:
	ActorShootAtEntityRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Entity shotAtEntity);
	Entity getShotAtEntity();
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
	virtual void executeNativesAfterRecording(Actor actor) override;

};

class ActorShootAtByImpactRecordingItem : public ActorRecordingItem {
protected:
	Hash m_weapon;
	Vector3 m_weaponImpact;
	Hash m_firingPattern;
	float m_walkSpeed;
	float m_heading;
public:
	ActorShootAtByImpactRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Hash weapon, Vector3 weaponImpact, Hash firingPattern, float walkSpeed, float heading);
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
	void executeNativesAfterRecording(Actor actor) override;

};

class ActorReloadRecordingItem : public ActorRecordingItem {
protected:
	Hash m_weapon;
	bool m_doAim;
	Vector3 m_weaponImpact;
public:
	ActorReloadRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Hash weapon, bool doAim, Vector3 weaponImpact);
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;

};

class ActorScenarioRecordingItem : public ActorRecordingItem {
protected:
	Scenario m_scenario;

public:
	ActorScenarioRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Scenario scenario);
	Scenario getScenario();
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
	void executeNativesAfterRecording(Actor actor) override;

};

class ActorVehicleRecordingItem : public ActorRecordingItem {
protected:
	Vehicle m_vehicle;
	float m_vehicleHeading;
	VEHICLE_TYPE m_vehicleType;
	VEHICLE_TYPE _getVehicleTypeFromNatives();
public:
	ActorVehicleRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading);
	Vehicle getVehicle();
	VEHICLE_TYPE getVehicleType();
	float getVehicleHeading();
	std::string toString() override;
};

class ActorVehicleEnterRecordingItem : public ActorVehicleRecordingItem {
protected:
	int m_vehicleSeat;
	float m_enterVehicleSpeed;
public:
	ActorVehicleEnterRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading, int vehicleSeat,float enterVehicleSpeed);
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
};

class ActorVehicleExitRecordingItem : public ActorVehicleRecordingItem {
protected:
	int m_vehicleSeat;
	float m_enterVehicleSpeed;
public:
	ActorVehicleExitRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading);
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
};

class ActorVehicleMovementRecordingItem : public ActorVehicleRecordingItem {
protected:
	float m_speedInVehicle;
public:
	ActorVehicleMovementRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading, float speedInVehicle);
	std::string toString() override;
	float getSpeedInVehicle();
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
};


class ActorAnimationSequenceRecordingItem : public ActorRecordingItem {
protected:
	AnimationSequence m_animationSequence;
	AnimationFlag m_animationFlag;
public:
	ActorAnimationSequenceRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, AnimationSequence animationSequence, AnimationFlag animationFlag);
	AnimationSequence getAnimationSequence();
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;

};

class ActorCoverAtRecordingItem : public ActorRecordingItem {
protected:
	Vector3 m_enterCoverPosition;
	Vector3 m_coverPosition;
public:
	ActorCoverAtRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vector3 enterCoverPosition, Vector3 coverPosition);
	void setCoverPosition(Vector3 coverPosition);
	std::string toString() override;
	virtual void executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)override;
	bool isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) override;
};


class ActorRecordingPlayback {
protected:
	bool m_hasFirstItemPlayback = false;;
	bool m_playbackCompleted = false;
	bool m_hasTeleportedToStartLocation = false;
	int m_recordingItemIndex;
	int m_maxRecordingItemIndex;
	int m_attemptsCheckCompletion;
	DWORD m_ticksStartCurrentItem;
	DWORD m_ticksLastCheckOfCurrentItem;
	DWORD m_ticksPlaybackStarted;
	DWORD m_ticksTeleportStartLocation;
public:
	ActorRecordingPlayback();
	ActorRecordingPlayback(DWORD tickStart, int nrOfRecordedItems);

	DWORD getTicksPlaybackStarted();

	void setRecordingItemIndex(int index);
	void nextRecordingItemIndex(DWORD ticksNow);
	bool isCurrentRecordedItemLast();
	int getRecordedItemIndex();
	int getNumberOfRecordedItems();

	int getAttemptsCheckedCompletion();
	void incrementAttempstCheckedCompletion();


	void setPlaybackCompleted();
	bool hasPlaybackCompleted();

	void setHasTeleportedToStartLocation(DWORD ticksNow);
	bool hasTeleportedToStartLocation();
	DWORD getTicksTeleportedToStartLocation();

	void setTicksLastCheckOfCurrentItem(DWORD ticks);
	DWORD getTicksLastCheckOfCurrentItem();

	DWORD getTicksStartCurrentItem();

	void setHasFirstItemPlayback(bool hasPlaybacked);
	bool getHasFirstItemPlayback();

	std::string toString();
};
