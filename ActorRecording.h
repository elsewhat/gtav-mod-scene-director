#pragma once
#include <vector>
#include "..\..\inc\types.h"
#include "script.h"
#include <string>

class ActorRecordingItem {
protected:
	DWORD m_ticksAfterRecordStart;
	Ped m_actor;
public:
	ActorRecordingItem(DWORD ticksStart, Ped actor);

	DWORD getTicksAfterRecordStart();
	std::string to_string();
};

class ActorMovementRecordingItem : public ActorRecordingItem {
protected:
	Vector3 m_location;
	float m_heading;
public:
	ActorMovementRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, float heading);
	Vector3 getLocation();
	float getHeading();
	std::string to_string();

};

class ActorVehicleRecordingItem : public ActorRecordingItem {
protected:
	Vector3 m_location;
	Vehicle m_vehicle;
	VEHICLE_TYPE m_vehicleType;
	VEHICLE_TYPE _getVehicleTypeFromNatives();
public:
	ActorVehicleRecordingItem(DWORD ticksStart, Ped actor, Vehicle veh, Vector3 location);
	Vector3 getLocation();
	Vehicle getVehicle();
	VEHICLE_TYPE getVehicleType();
	std::string to_string();
};

class ActorRecordingPlayback {
protected:
	int m_recordingItemIndex;
	DWORD m_tickStartCurrentItem;
public:
	ActorRecordingPlayback();
	ActorRecordingPlayback(DWORD tickStart);

	DWORD getTicksAfterRecordStart();

	void setRecordingItemIndex(int index);
	int getRecordingItemIndex();
};
