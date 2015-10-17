#include <vector>
#include <algorithm>
#include "ActorRecording.h"
#include "script.h"
#include "Actor.h"
#include "scenario.h"

ActorRecordingItem::ActorRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location)
{
	m_ticksAfterRecordStart = ticksStart;
	m_actorPed = actor;
	m_location = location;
	m_ticksDeltaCheckCompletion = 300;
	m_ticksDeltaWhenRecorded = ticksDeltaWhenRecorded;
}

DWORD ActorRecordingItem::getTicksAfterRecordStart()
{
	return m_ticksAfterRecordStart;
}


Vector3 ActorRecordingItem::getLocation()
{
	return m_location;
}




DWORD ActorRecordingItem::getTicksDeltaCheckCompletion()
{
	return m_ticksDeltaCheckCompletion;
}

void ActorRecordingItem::setTicksLength(DWORD ticks)
{
	m_ticksLength = ticks;
}

DWORD ActorRecordingItem::getTicksLength()
{
	return m_ticksLength;
}

std::string ActorRecordingItem::toString()
{
	return "ActorRecordingItem " + std::to_string(m_actorPed) + " ticks: " + std::to_string(m_ticksAfterRecordStart) + " delta_ticks:" + std::to_string(m_ticksDeltaWhenRecorded);
}

void ActorRecordingItem::executeNativesAfterRecording(Actor actor)
{
}

ActorOnFootMovementRecordingItem::ActorOnFootMovementRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed):ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_walkSpeed = walkSpeed;
}

float ActorOnFootMovementRecordingItem::getWalkSpeed()
{
	return m_walkSpeed;
}

void ActorOnFootMovementRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_GO_STRAIGHT_TO_COORD(m_actorPed, m_location.x, m_location.y, m_location.z, m_walkSpeed, -1, 27.0f, 0.5f);
}

bool ActorOnFootMovementRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	if (ticksNow - ticksStart >= (m_ticksDeltaWhenRecorded / 2.0)) {
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
}


std::string ActorOnFootMovementRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorMovementRecordingItem Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ") Speed " + std::to_string(m_walkSpeed);
}


ActorVehicleRecordingItem::ActorVehicleRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh) :ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
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
	m_ticksStartCurrentItem = ticksNow;
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

void ActorRecordingPlayback::setTicksLastCheckOfCurrentItem(DWORD ticks)
{
	m_ticksLastCheckOfCurrentItem = ticks;
}

DWORD ActorRecordingPlayback::getTicksLastCheckOfCurrentItem()
{
	return m_ticksLastCheckOfCurrentItem;
}

DWORD ActorRecordingPlayback::getTicksStartCurrentItem()
{
	return m_ticksStartCurrentItem;
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

ActorVehicleEnterRecordingItem::ActorVehicleEnterRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, int vehicleSeat, float enterVehicleSpeed) : ActorVehicleRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, veh)
{
	m_vehicleSeat = vehicleSeat;
	m_enterVehicleSpeed = enterVehicleSpeed;
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

std::string ActorVehicleEnterRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleEnterRecordingItem Vehicle " + std::to_string(m_vehicle) + " Seat : " + std::to_string(m_vehicleSeat) + " Speed:" + std::to_string(m_enterVehicleSpeed);
}

void ActorVehicleEnterRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_ENTER_VEHICLE(m_actorPed, m_vehicle, -1, m_vehicleSeat, m_enterVehicleSpeed, 1, 0);
}

bool ActorVehicleEnterRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	if (PED::IS_PED_SITTING_IN_VEHICLE(m_actorPed,m_vehicle)) {
		return true;
	}
	else {
		return false;
	}
}

ActorVehicleExitRecordingItem::ActorVehicleExitRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh): ActorVehicleRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, veh)
{
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

std::string ActorVehicleExitRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleExitRecordingItem Vehicle " + std::to_string(m_vehicle);
}

void ActorVehicleExitRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_LEAVE_VEHICLE(m_actorPed, m_vehicle, 0);
}

bool ActorVehicleExitRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
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

ActorStandingStillRecordingItem::ActorStandingStillRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float heading) :ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_heading = heading;
	m_ticksDeltaCheckCompletion = 200;
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

bool ActorStandingStillRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	//will first be checked after m_ticksDeltaCheckCompletion
	return true;
}

ActorVehicleMovementRecordingItem::ActorVehicleMovementRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float speedInVehicle) : ActorVehicleRecordingItem(ticksStart,  ticksDeltaWhenRecorded, actor, location, veh)
{
	m_speedInVehicle = speedInVehicle;
}

std::string ActorVehicleMovementRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleMovementRecordingItem Vehicle " + std::to_string(m_vehicle) + " speed : " + std::to_string(m_speedInVehicle);
}

float ActorVehicleMovementRecordingItem::getSpeedInVehicle()
{
	return m_speedInVehicle;
}

void ActorVehicleMovementRecordingItem::executeNativesForRecording(Actor actor)
{
	if (PED::IS_PED_IN_ANY_VEHICLE(m_actorPed, 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(m_actorPed);

		//we assume pedVehicle is the right vehicle

		//check if player is the driver
		Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
		if (pedDriver != m_actorPed) {
			log_to_file("ActorVehicleMovementRecordingItem: Actor (" + std::to_string(m_actorPed) + " is not driver (" + std::to_string(pedDriver) + ") Will do no action");
			//set_status_text("Ped is not driver. Ignore waypoint");
		}
		else {

			if (PED::IS_PED_IN_ANY_HELI(m_actorPed)) {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, m_location.x, m_location.y, m_location.z, m_speedInVehicle, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
				log_to_file("playback_recording_to_waypoint: Flying in heli with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));
			}
			else if (PED::IS_PED_IN_ANY_PLANE(m_actorPed)) {
				AI::TASK_PLANE_MISSION(pedDriver, pedVehicle, 0, 0, m_location.x, m_location.y, m_location.z, 4, 30.0, 50.0, -1, m_speedInVehicle, 50);
				log_to_file("playback_recording_to_waypoint: Flying in plane with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));

			}
			else if (PED::IS_PED_IN_ANY_BOAT(m_actorPed)) {
				AI::TASK_BOAT_MISSION(pedDriver, pedVehicle, 0, 0, m_location.x, m_location.y, m_location.z, 4, m_speedInVehicle, 786469, -1.0, 7);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(pedDriver, 1);
				//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, 0.0, 20.0, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786469, 5.0, 1071);
				log_to_file("playback_recording_to_waypoint: In boat : " + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));
			}
			else {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, m_location.x, m_location.y, m_location.z, m_speedInVehicle, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), actor.getDrivingMode().value, 5.0, -1);
				log_to_file("playback_recording_to_waypoint: Driving with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));

			}
		}

	}

}

bool ActorVehicleMovementRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	if (ticksNow - ticksStart >= (m_ticksDeltaWhenRecorded / 2.0)) {
		//log_to_file("ticksNow - ticksStart = " + std::to_string(ticksNow - ticksStart) + " (m_ticksDeltaCheckCompletion / 2.0) = " + std::to_string((m_ticksDeltaWhenRecorded / 2.0)));
		if (PED::IS_PED_IN_ANY_VEHICLE(m_actorPed, 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(m_actorPed);

			bool isInVehicle = PED::IS_PED_IN_ANY_VEHICLE(m_actorPed, 0);
			bool isPedInHeli = PED::IS_PED_IN_ANY_HELI(m_actorPed);
			bool isPedInPlane = PED::IS_PED_IN_ANY_PLANE(m_actorPed);
			bool isPedInBoat = PED::IS_PED_IN_ANY_BOAT(m_actorPed);

			float minDistance = 4.0;

			if (isPedInHeli) {
				minDistance = 90.0;
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
			log_to_file("ActorVehicleMovementRecordingItem: Distance to target: " + std::to_string(distanceToTarget));

			if (distanceToTarget < minDistance) {
				log_to_file("ActorVehicleMovementRecordingItem: Close enough to target for ActorRecordingItem");
				return true;
			}
			else {
				return false;
			}

		}
		else {
			//not in any vehicle so return true
			log_to_file("ActorVehicleMovementRecordingItem: Not in any vehicle so returning true isRecordingItemCompleted");
			return true;
		}
	} else {
		log_to_file("Waiting " + std::to_string((m_ticksDeltaCheckCompletion / 2.0)) + " before checking distance to target for completion");
	}
}

ActorScenarioRecordingItem::ActorScenarioRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Scenario scenario): ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_scenario = scenario;
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

Scenario ActorScenarioRecordingItem::getScenario()
{
	return m_scenario;
}

std::string ActorScenarioRecordingItem::toString()
{
	log_to_file("ActorScenarioRecordingItem: Enter toString");
	return ActorRecordingItem::toString() + " ActorScenarioRecordingItem Scenario: " +m_scenario.name + " Length: " + std::to_string(m_ticksLength);
}

void ActorScenarioRecordingItem::executeNativesForRecording(Actor actor)
{
	if (m_scenario.hasEnterAnim) {
		AI::TASK_START_SCENARIO_IN_PLACE(m_actorPed, m_scenario.name, -1, 1);
	}
	else {
		AI::TASK_START_SCENARIO_IN_PLACE(m_actorPed, m_scenario.name, -1, 0);
	}
}

bool ActorScenarioRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	if (ticksNow - ticksStart >= m_ticksLength) {
		return true;
	}
	else {
		return false;
	}

}

void ActorScenarioRecordingItem::executeNativesAfterRecording(Actor actor)
{
	log_to_file("ActorScenarioRecordingItem: executeNativesAfterRecording calling AI::CLEAR_PED_TASKS");
	AI::CLEAR_PED_TASKS(m_actorPed);
}

ActorAimAtRecordingItem::ActorAimAtRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Entity aimedAtEntity): ActorRecordingItem( ticksStart, ticksDeltaWhenRecorded, actor,  location)
{
	m_aimedAtEntity = aimedAtEntity;
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

Entity ActorAimAtRecordingItem::getAimedAtEntity()
{
	return m_aimedAtEntity;
}

std::string ActorAimAtRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorAimAtRecordingItem Entity " + std::to_string(m_aimedAtEntity) ;
}

void ActorAimAtRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_AIM_GUN_AT_ENTITY(m_actorPed, m_aimedAtEntity, -1, 0);
}

bool ActorAimAtRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	if (ticksNow - ticksStart >= m_ticksLength) {
		return true;
	}
	else {
		return false;
	}
}

void ActorAimAtRecordingItem::executeNativesAfterRecording(Actor actor)
{
	AI::CLEAR_PED_TASKS(actor.getActorPed());
}

ActorShootAtRecordingItem::ActorShootAtRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Entity shotAtEntity) :ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor,location)
{
	m_shotAtEntity = shotAtEntity;
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

Entity ActorShootAtRecordingItem::getShotAtEntity()
{
	return m_shotAtEntity;
}

std::string ActorShootAtRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorShootAtRecordingItem Entity " + std::to_string(m_shotAtEntity);
}

void ActorShootAtRecordingItem::executeNativesForRecording(Actor actor)
{
	AI::TASK_SHOOT_AT_ENTITY(m_actorPed, m_shotAtEntity, -1, GAMEPLAY::GET_HASH_KEY("FIRING_PATTERN_SINGLE_SHOT"));
}

bool ActorShootAtRecordingItem::isRecordingItemCompleted(DWORD ticksStart, DWORD ticksNow, Actor actor, Vector3 location)
{
	return true;
}

void ActorShootAtRecordingItem::executeNativesAfterRecording(Actor actor)
{
	AI::CLEAR_PED_TASKS(actor.getActorPed());
}
