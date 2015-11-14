#include "actor.h"
#include <vector>
#include <algorithm>


Actor::Actor()
{
	m_ped = 0;
}

Actor::Actor(Ped ped) {
	m_ped = ped;
	setRelationshipGroup(getDefaultRelationshipGroup());
	setDrivingMode(getDefaultDrivingMode());
}

Ped Actor::getActorPed()
{
	return m_ped;
}

Actor Actor::nullActor()
{
	return Actor(0);
}

bool Actor::isNullActor()
{
	if (m_ped == 0) {
		return true;
	}
	else {
		return false;
	}
}

bool Actor::isActorThisPed(Ped ped)
{
	if (m_ped == ped) {
		return true;
	}
	else {
		return false;
	}
}

void Actor::setActiveInScene(SCENE_MODE mode)
{
	m_sceneStatus = mode;
}

SCENE_MODE Actor::isActiveInScene()
{
	return m_sceneStatus;
}

void Actor::setBlipId(int blipId)
{
	m_blipId = blipId;
}

int Actor::getBlipId()
{
	return m_blipId;
}

void Actor::setHasWaypoint(bool hasWaypoint)
{
	m_hasWaypoint = hasWaypoint;
}

bool Actor::hasWaypoint()
{
	return m_hasWaypoint;
}

void Actor::setWaypoint(Vector3 location)
{
	m_waypoint = location;
}

Vector3 Actor::getWaypoint()
{
	return m_waypoint;
}

void Actor::setHasStartLocation(bool hasStartLocation)
{
	m_hasStartLocation = hasStartLocation;
}

bool Actor::hasStartLocation()
{
	return m_hasStartLocation;
}

void Actor::setStartLocation(Vector3 location)
{
	m_startLocation = location;
}

Vector3 Actor::getStartLocation()
{
	return m_startLocation;
}

void Actor::setStartLocationHeading(float heading)
{
	m_startLocationHeading = heading;
}

float Actor::getStartLocationHeading()
{
	return m_startLocationHeading;
}

void Actor::setHasSpotLight(bool hasSpot)
{
	m_hasSpotlight = hasSpot;
}

bool Actor::hasSpotLight()
{
	return m_hasSpotlight;
}

void Actor::setSpotLightType(SPOT_LIGHT_TYPE type)
{
	m_spotlightType = type;
}

SPOT_LIGHT_TYPE Actor::getSpotLightType()
{
	return m_spotlightType;
}

void Actor::setSpotLightColor(SpotLightColor color)
{
	m_spotlightColor = color;
}

SpotLightColor Actor::getSpotLightColor()
{
	return m_spotlightColor;
}

void Actor::setHasWalkingStyle(bool hasWalkingStyle)
{
	m_hasWalkingStyle = hasWalkingStyle;
}

bool Actor::hasWalkingStyle()
{
	return m_hasWalkingStyle;
}

void Actor::setWalkingStyle(ClipSet walkingStyle)
{
	m_walkingStyle = walkingStyle;
}

ClipSet Actor::getWalkingStyle()
{
	return m_walkingStyle;
}

void Actor::setRelationshipGroup(RelationshipGroup group)
{
	m_relationshipGroup = group;
}

RelationshipGroup Actor::getRelationshipGroup()
{
	return m_relationshipGroup;
}

void Actor::setVehicleNoDamage(bool noDamage)
{
	m_vehicleNoDamage = noDamage;
}

bool Actor::hasVehicleNoDamage()
{
	return m_vehicleNoDamage;
}

void Actor::setWalkingSpeed(float speed)
{
	m_walkingSpeed = speed;
}

float Actor::getWalkingSpeed()
{
	return m_walkingSpeed;
}

void Actor::setDrivingMode(DrivingMode drivingMode)
{
	m_drivingMode = drivingMode;
}

DrivingMode Actor::getDrivingMode()
{
	return m_drivingMode;
}

void Actor::setHasRecording(bool hasRecording)
{
	m_hasRecording = hasRecording;
}

void Actor::startReplayRecording(DWORD tickStart)
{
	m_replayRecordingInProgress = true;
	m_actorRecordingPlayback = ActorRecordingPlayback(tickStart, (int)m_actorRecordingItems.size());
}

void Actor::stopReplayRecording()
{
	m_replayRecordingInProgress = false;
}

void Actor::resumeReplayRecording()
{
	m_replayRecordingInProgress = true;
}

bool Actor::isCurrentlyPlayingRecording()
{
	return m_replayRecordingInProgress;
}

void Actor::removeRecording()
{
	m_hasRecording = false;
	m_hasStartLocation = false;
	m_startLocation = Vector3();

}

ActorRecordingPlayback& Actor::getRecordingPlayback()
{
	return m_actorRecordingPlayback;
}

void Actor::setRecordingPlayback(ActorRecordingPlayback recordingPlayback)
{
	m_actorRecordingPlayback = recordingPlayback;
}

bool Actor::hasRecording()
{
	return m_hasRecording;
}

void Actor::setStartLocationVehicle(Vehicle vehicle, Vector3 startLocationVehicleLocation, float startLocationVehicleHeading)
{
	m_startLocationVehicle = vehicle;
	m_startLocationVehicleLocation = startLocationVehicleLocation;
	m_startLocationVehicleHeading = startLocationVehicleHeading;
}

Vector3 Actor::getStartLocationVehicleLocation() {
	return m_startLocationVehicleLocation;
}

Vehicle Actor::getStartLocationVehicle()
{
	return m_startLocationVehicle;
}

float Actor::getStartLocationVehicleHeading()
{
	return m_startLocationVehicleHeading;
}

bool Actor::hasStartLocationVehicle() {
	if (m_startLocationVehicle != 0) {
		return true;
	}
	else {
		return false;
	}
}

void Actor::setStartLocationVehicleSeat(int seat)
{
	m_startLocationVehicleSeat = seat;
}

int Actor::getStartLocationVehicleSeat()
{
	return m_startLocationVehicleSeat;
}

void Actor::setRecording(std::vector<std::shared_ptr<ActorRecordingItem>> actorRecordingItems)
{
	m_actorRecordingItems = actorRecordingItems;
}

std::vector<std::shared_ptr<ActorRecordingItem>> Actor::getRecording()
{
	return m_actorRecordingItems;
}

std::shared_ptr<ActorRecordingItem> Actor::getRecordingAt(int index)
{
	return m_actorRecordingItems[index];
}

