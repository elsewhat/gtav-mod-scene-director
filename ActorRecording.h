#pragma once
#include <vector>
#include "..\..\inc\types.h"
#include "script.h"
#include <string>

//forward declaration
class Actor;

class ActorRecordingItem {
protected:
	DWORD m_ticksAfterRecordStart;
	Ped m_actor;
	Vector3 m_location;
	DWORD m_ticksDeltaCheckCompletion;
public:
	ActorRecordingItem(DWORD ticksStart, Ped actor, Vector3 location);

	DWORD getTicksAfterRecordStart();
	Vector3 getLocation();
	void executeNativesForRecording(Actor actor);
	bool isRecordingItemCompleted(Actor actor, Vector3 location);

	DWORD getTicksDeltaCheckCompletion();

	std::string toString();
};

class ActorMovementRecordingItem : public ActorRecordingItem {
protected:
	float m_heading;
public:
	ActorMovementRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, float heading);
	float getHeading();
	std::string toString();

};

class ActorVehicleRecordingItem : public ActorRecordingItem {
protected:
	Vehicle m_vehicle;
	VEHICLE_TYPE m_vehicleType;
	VEHICLE_TYPE _getVehicleTypeFromNatives();
public:
	ActorVehicleRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, Vehicle veh);
	Vehicle getVehicle();
	VEHICLE_TYPE getVehicleType();
	std::string toString();
};

class ActorRecordingPlayback {
protected:
	bool m_hasFirstItemPlayback = false;;
	bool m_playbackCompleted = false;
	bool m_hasTeleportedToStartLocation = false;
	int m_recordingItemIndex;
	int m_maxRecordingItemIndex;
	DWORD m_ticksStartCurrentItem;
	DWORD m_ticksLastCheckOfCurrentItem;
	DWORD m_ticksPlaybackStarted;
	DWORD m_ticksTeleportStartLocation;
public:
	ActorRecordingPlayback();
	ActorRecordingPlayback(DWORD tickStart, int nrOfRecordedItems);

	DWORD getTicksAfterCurrentItemStarted();
	DWORD getTicksPlaybackStarted();

	void setRecordingItemIndex(int index);
	void nextRecordingItemIndex(DWORD ticksNow);
	bool isCurrentRecordedItemLast();
	int getRecordedItemIndex();
	int getNumberOfRecordedItems();

	void setPlaybackCompleted();
	bool hasPlaybackCompleted();

	void setHasTeleportedToStartLocation(DWORD ticksNow);
	bool hasTeleportedToStartLocation();
	DWORD getTicksTeleportedToStartLocation();

	void setTickLastCheckOfCurrentItem(DWORD ticks);
	DWORD getTickLastCheckOfCurrentItem();

	void setHasFirstItemPlayback(bool hasPlaybacked);
	bool getHasFirstItemPlayback();

	std::string toString();
};
