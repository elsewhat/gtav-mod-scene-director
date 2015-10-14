#include <vector>
#include <algorithm>
#include "ActorRecording.h"
#include "script.h"
#include "Actor.h"

ActorRecordingItem::ActorRecordingItem(DWORD ticksStart, Ped actor, Vector3 location)
{
	m_ticksAfterRecordStart = ticksStart;
	m_actorPed = actor;
	m_location = location;
	m_ticksDeltaCheckCompletion = 300;
}

DWORD ActorRecordingItem::getTicksAfterRecordStart()
{
	return m_ticksAfterRecordStart;
}


Vector3 ActorRecordingItem::getLocation()
{
	return m_location;
}

void ActorRecordingItem::executeNativesForRecording(Actor actor)
{
	playback_recording_to_waypoint(actor.getActorPed(), m_location);
}

bool ActorRecordingItem::isRecordingItemCompleted(Actor actor, Vector3 location)
{
	Ped actorPed = actor.getActorPed();
	bool isInVehicle = PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0);
	bool isPedInHeli = PED::IS_PED_IN_ANY_HELI(actorPed);
	bool isPedInPlane = PED::IS_PED_IN_ANY_PLANE(actorPed);
	bool isPedInBoat = PED::IS_PED_IN_ANY_BOAT(actorPed);

	float minDistance = 4.0;

	if (isPedInHeli) {
		minDistance = 50.0;
	}
	else if (isPedInPlane) {
		minDistance = 100.0;
	}
	else if (isPedInBoat) {
		minDistance = 60.0;
	}
	else if (isInVehicle) {
		minDistance = 15.0;
	}

	float distanceToTarget = SYSTEM::VDIST(m_location.x, m_location.y, m_location.z, location.x, location.y, location.z);
	log_to_file("ActorRecordingItem: Distance to target: " + std::to_string(distanceToTarget));

	if (distanceToTarget < minDistance) {
		log_to_file("ActorRecordingItem: Close enough to target for ActorRecordingItem");
		return true;
	}
	else {
		return false;
	}
}

DWORD ActorRecordingItem::getTicksDeltaCheckCompletion()
{
	return m_ticksDeltaCheckCompletion;
}

std::string ActorRecordingItem::toString()
{
	return "Recording start for actor " + std::to_string(m_actorPed) + " at " + std::to_string(m_ticksAfterRecordStart);
}

ActorMovementRecordingItem::ActorMovementRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, float walkSpeed):ActorRecordingItem(ticksStart, actor, location)
{
	m_walkSpeed = walkSpeed;
}

float ActorMovementRecordingItem::getWalkSpeed()
{
	return m_walkSpeed;
}

void ActorMovementRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_GO_STRAIGHT_TO_COORD(m_actorPed, m_location.x, m_location.y, m_location.z, m_walkSpeed, -1, 27.0f, 0.5f);
}

bool ActorMovementRecordingItem::isRecordingItemCompleted(Actor actor, Vector3 location)
{
	float minDistance = 4.0;
	if (m_walkSpeed > 1.0) {
		minDistance = 7.0;
	}
	float distanceToTarget = SYSTEM::VDIST(m_location.x, m_location.y, m_location.z, location.x, location.y, location.z);
	log_to_file("ActorMovementRecordingItem: Distance to target: " + std::to_string(distanceToTarget) + " Min distance: " + std::to_string(minDistance));

	if (distanceToTarget < minDistance) {
		log_to_file("ActorMovementRecordingItem: Close enough to target for ActorRecordingItem");
		return true;
	}
	else {
		return false;
	}
}


std::string ActorMovementRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorMovementRecordingItem Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ") Speed " + std::to_string(m_walkSpeed);
}


ActorVehicleRecordingItem::ActorVehicleRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, Vehicle veh) :ActorRecordingItem(ticksStart, actor, location)
{
	m_vehicle = veh;
	m_vehicleType = _getVehicleTypeFromNatives();
}


Vehicle ActorVehicleRecordingItem::getVehicle()
{
	return m_vehicle;
}

VEHICLE_TYPE ActorVehicleRecordingItem::getVehicleType()
{
	return m_vehicleType;
}

std::string ActorVehicleRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleRecordingItem Vehicle " + std::to_string(m_vehicle) + " Vehicle type " + std::to_string(m_vehicleType) + " Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ")";
}

VEHICLE_TYPE ActorVehicleRecordingItem::_getVehicleTypeFromNatives()
{
	//natives for VEHICLE all require the Hash of the model, so checking actor instead
	if (PED::IS_PED_IN_ANY_HELI(m_actorPed)) {
		return VEHICLE_TYPE_HELI;
	}
	else if (PED::IS_PED_IN_ANY_PLANE(m_actorPed)) {
		return VEHICLE_TYPE_PLANE;
	}
	else if (PED::IS_PED_IN_ANY_BOAT(m_actorPed)) {
		return VEHICLE_TYPE_BOAT;
	}
	else if (PED::IS_PED_IN_ANY_SUB(m_actorPed)) {
		return VEHICLE_TYPE_SUB;
	}
	else if (PED::IS_PED_ON_ANY_BIKE(m_actorPed)){
		return VEHICLE_TYPE_BIKE;
	}
	else {
		return VEHICLE_TYPE_CAR;
	}
}

ActorRecordingPlayback::ActorRecordingPlayback()
{
	m_recordingItemIndex = 0;
	m_ticksStartCurrentItem = 0;
}

ActorRecordingPlayback::ActorRecordingPlayback(DWORD tickStart, int maxRecordingItemIndex)
{
	m_recordingItemIndex = 0;
	m_ticksStartCurrentItem = tickStart;
	m_ticksPlaybackStarted = tickStart;
	m_maxRecordingItemIndex = maxRecordingItemIndex;
}

DWORD ActorRecordingPlayback::getTicksAfterCurrentItemStarted()
{
	return m_ticksStartCurrentItem;
}

DWORD ActorRecordingPlayback::getTicksPlaybackStarted()
{
	return m_ticksPlaybackStarted;
}

void ActorRecordingPlayback::setRecordingItemIndex(int index)
{
	m_recordingItemIndex = index;
}

void ActorRecordingPlayback::nextRecordingItemIndex(DWORD ticksNow)
{
	m_recordingItemIndex = m_recordingItemIndex+1;
	m_ticksLastCheckOfCurrentItem = ticksNow;
}

bool ActorRecordingPlayback::isCurrentRecordedItemLast()
{
	if (m_recordingItemIndex + 1 >= m_maxRecordingItemIndex) {
		return true;
	}
	else {
		return false;
	}

}

int ActorRecordingPlayback::getRecordedItemIndex()
{
	return m_recordingItemIndex;
}

int ActorRecordingPlayback::getNumberOfRecordedItems()
{
	return m_maxRecordingItemIndex;
}

void ActorRecordingPlayback::setPlaybackCompleted()
{
	m_playbackCompleted = true;
}

bool ActorRecordingPlayback::hasPlaybackCompleted()
{
	return m_playbackCompleted;
}

void ActorRecordingPlayback::setHasTeleportedToStartLocation(DWORD ticksNow)
{
	m_hasTeleportedToStartLocation = true;
	m_ticksTeleportStartLocation = ticksNow;
}


bool ActorRecordingPlayback::hasTeleportedToStartLocation()
{
	return m_hasTeleportedToStartLocation;
}

DWORD ActorRecordingPlayback::getTicksTeleportedToStartLocation()
{
	return m_ticksTeleportStartLocation;
}

void ActorRecordingPlayback::setTickLastCheckOfCurrentItem(DWORD ticks)
{
	m_ticksLastCheckOfCurrentItem = ticks;
}

DWORD ActorRecordingPlayback::getTickLastCheckOfCurrentItem()
{
	return m_ticksLastCheckOfCurrentItem;
}

void ActorRecordingPlayback::setHasFirstItemPlayback(bool hasPlaybacked)
{
	m_hasFirstItemPlayback = hasPlaybacked;
}

bool ActorRecordingPlayback::getHasFirstItemPlayback()
{
	return m_hasFirstItemPlayback;
}

std::string ActorRecordingPlayback::toString()
{
	return "ActorRecordingPlayback getHasFirstItemPlayback()=" + std::to_string(getHasFirstItemPlayback()) + " getRecordedItemIndex()" + std::to_string(getRecordedItemIndex());
}

ActorVehicleEnterRecordingItem::ActorVehicleEnterRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, Vehicle veh, int vehicleSeat, float enterVehicleSpeed) : ActorVehicleRecordingItem(ticksStart, actor, location, veh)
{
	m_vehicleSeat = vehicleSeat;
	m_enterVehicleSpeed = enterVehicleSpeed;
}

std::string ActorVehicleEnterRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleEnterRecordingItem Vehicle " + std::to_string(m_vehicle) + " Seat : " + std::to_string(m_vehicleSeat) + " Speed:" + std::to_string(m_enterVehicleSpeed);
}

void ActorVehicleEnterRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_ENTER_VEHICLE(m_actorPed, m_vehicle, -1, m_vehicleSeat, m_enterVehicleSpeed, 1, 0);
}

bool ActorVehicleEnterRecordingItem::isRecordingItemCompleted(Actor actor, Vector3 location)
{
	if (PED::IS_PED_SITTING_IN_VEHICLE(m_actorPed,m_vehicle)) {
		return true;
	}
	else {
		return false;
	}
}

ActorVehicleExitRecordingItem::ActorVehicleExitRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, Vehicle veh): ActorVehicleRecordingItem(ticksStart, actor, location, veh)
{
}

std::string ActorVehicleExitRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleExitRecordingItem Vehicle " + std::to_string(m_vehicle);
}

void ActorVehicleExitRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_LEAVE_VEHICLE(m_actorPed, m_vehicle, 0);
}

bool ActorVehicleExitRecordingItem::isRecordingItemCompleted(Actor actor, Vector3 location)
{
	if (PED::IS_PED_SITTING_IN_VEHICLE(m_actorPed, m_vehicle)) {
		return false;
	}
	else {//extra check in order to make sure they are not in the vehicle
		if (PED::IS_PED_IN_ANY_VEHICLE(m_actorPed, 0) && PED::GET_VEHICLE_PED_IS_USING(m_actorPed) == m_vehicle) {
			return false;
		}
		else {
			return true;
		}
	}
}

ActorStandingStillRecordingItem::ActorStandingStillRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, float heading) :ActorRecordingItem(ticksStart, actor, location)
{
	m_heading = heading;
	m_ticksDeltaCheckCompletion = 1000;
}

float ActorStandingStillRecordingItem::getHeading()
{
	return m_heading;
}

std::string ActorStandingStillRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorStandingStillRecordingItem Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ") Heading " + std::to_string(m_heading);
}

void ActorStandingStillRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::CLEAR_PED_TASKS(m_actorPed);
}

bool ActorStandingStillRecordingItem::isRecordingItemCompleted(Actor actor, Vector3 location)
{
	//will first be checked after m_ticksDeltaCheckCompletion
	return true;
}
