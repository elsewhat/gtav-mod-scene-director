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
	m_startLocationVehicle = 0;
	m_recordingDelay = 0;
}

Ped Actor::getActorPed()
{
	return m_ped;
}

std::string Actor::getName()
{
	return m_name;
}

void Actor::setName(std::string name)
{
	m_name = name;
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

void Actor::changeActorPed(Ped newPed)
{
	m_ped = newPed;
}

bool Actor::isActorInVehicle(Vehicle veh)
{
	if (PED::IS_PED_IN_ANY_VEHICLE(m_ped, 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(m_ped);

		if (pedVehicle == veh) {
			return true;
		}
	}
	return false;
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
	getRecordingPlayback().setRecordingItemIndex(0);
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
	m_startLocationVehicle = 0;
	m_startLocation = Vector3();
	m_hasRecordingWithGunFire = false;

}

ActorRecordingPlayback& Actor::getRecordingPlayback()
{
	return m_actorRecordingPlayback;
}

void Actor::setRecordingPlayback(ActorRecordingPlayback recordingPlayback)
{
	m_actorRecordingPlayback = recordingPlayback;
}


void Actor::drawMarkersForRecording(bool showDisabled)
{
	if (hasRecording()) {
		int i = 0;
		for (std::shared_ptr<ActorRecordingItem> &recording_item : m_actorRecordingItems) {
			bool isCurrent = false;
			if (getRecordingPlayback().getRecordedItemIndex() == i) {
				isCurrent = true;
			}
			if (i >= getRecordingPlayback().getRecordedItemIndex()) {
				recording_item->drawMarkerForRecording(isCurrent, showDisabled);
			}
			i++;
		}
	}
}

float Actor::distanceToNearestLocation(Vector3 startingPoint)
{
	float minDistance = FLT_MAX;
	if (hasRecording()) {

		for (std::shared_ptr<ActorRecordingItem> &recording_item : m_actorRecordingItems) {
			Vector3 recordingLocation = recording_item->getLocation();
			float recDistance = SYSTEM::VDIST(startingPoint.x, startingPoint.y, startingPoint.z, recordingLocation.x, recordingLocation.y, recordingLocation.z);
			if (recDistance < minDistance) {
				minDistance = recDistance;
			}
		}
	}
	return minDistance;
}

std::shared_ptr<ActorRecordingItem> Actor::getNearestRecording(Vector3 startingPoint)
{
	if (hasRecording()) {
		float nearestDistance = FLT_MAX;
		std::shared_ptr<ActorRecordingItem> nearestRecording;
		int index = 1; 
		for (std::shared_ptr<ActorRecordingItem> &recording_item : m_actorRecordingItems) {
			recording_item->setMarkerType(MARKER_TYPE_NORMAL);
			recording_item->setIndex(index);

			Vector3 recordingLocation = recording_item->getLocation();
			float recDistance = SYSTEM::VDIST(startingPoint.x, startingPoint.y, startingPoint.z, recordingLocation.x, recordingLocation.y, recordingLocation.z);
			if (recDistance < nearestDistance) {
				nearestRecording = recording_item;
				nearestDistance = recDistance;
			}

			index++;
		}
		return nearestRecording;
	}
	else {
		return nullptr;
		//return std::shared_ptr<ActorRecordingItem>();
	}
	
}

bool Actor::hasRecording()
{
	return m_hasRecording;
}

void Actor::setHasRecordingWithGunFire(bool hasRecordingWithGunFire)
{
	m_hasRecordingWithGunFire = hasRecordingWithGunFire;
}

bool Actor::hasRecordingWithGunFire()
{
	return m_hasRecordingWithGunFire;
}

void Actor::setRecordingDelay(DWORD ticks)
{
	m_recordingDelay = ticks;
}

DWORD Actor::getRecordingDelay()
{
	return m_recordingDelay;
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
	log_to_file(std::to_string(m_ped) + " has start location vehicle value " + std::to_string(m_startLocationVehicle));
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
	if (index >= m_actorRecordingItems.size() || index< 0) {
		return nullptr;
	}
	else {
		return m_actorRecordingItems[index];
	}
	
}

void Actor::update_tick_recording_replay(Actor & actor) {
	Ped actorPed = actor.getActorPed();
	DWORD ticksNow = GetTickCount();

	//get the recording playback controller. Remember that this is by value and must be updated back to the actor
	ActorRecordingPlayback & recordingPlayback = actor.getRecordingPlayback();

	std::shared_ptr<ActorRecordingItem> recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
	//skip disabled recording items
	while (recordingItem->isDisabled()) {
		log_to_file("DISABLED " + recordingItem->toString());
		if (recordingPlayback.isCurrentRecordedItemLast()) {
			recordingItem = nullptr;
			break;
		}
		else {
			recordingPlayback.nextRecordingItemIndex(GetTickCount());
			recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
		}
	}
	//if we have only disabled items, stop the replay
	if(recordingItem==nullptr){
		recordingPlayback.setPlaybackCompleted();
		actor.stopReplayRecording();
		return;
	}

	//get next and previous recording items
	std::shared_ptr<ActorRecordingItem> nextRecordingItem;
	if (!recordingPlayback.isCurrentRecordedItemLast()) {
		nextRecordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex() + 1);
	}
	std::shared_ptr<ActorRecordingItem> previousRecordingItem;
	if (recordingPlayback.getRecordedItemIndex() >= 1) {
		previousRecordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex() - 1);
	}


	if (!recordingPlayback.hasTeleportedToStartLocation()) {
		log_to_file("update_tick_recording_replay - Initiate telport to start location for all actors");
		action_teleport_to_start_locations();

		//recordingPlayback.setHasTeleportedToStartLocation(ticksNow);
		return;
	}

	//special handling for the first item. Wait untill we start it
	if (!recordingPlayback.getHasFirstItemPlayback()) {

		if (ticksNow >= recordingPlayback.getTicksTeleportedToStartLocation() + 2000) {
			DWORD ticksPlaybackStart = recordingPlayback.getTicksPlaybackStarted();
			DWORD ticksDeltaStartFirst = recordingItem->getTicksAfterRecordStart();

			//log_to_file(actor.getName() + " Ticks now:" + std::to_string(ticksNow) + " ticksPlaybackStart: " + std::to_string(ticksPlaybackStart) + " ticksDeltaStartFirst:"+ std::to_string(ticksDeltaStartFirst) + " actor.getRecordingDelay() " + std::to_string(actor.getRecordingDelay()));

			if (ticksNow < ticksPlaybackStart + ticksDeltaStartFirst + actor.getRecordingDelay()) {
				return;
			}
			else {
				recordingPlayback.setHasFirstItemPlayback(true);
				recordingPlayback.setTicksLastCheckOfCurrentItem(ticksNow);
				log_to_file("Starting first recording item");
				log_to_file(recordingItem->toString());

				recordingItem->executeNativesForRecording(actor, nextRecordingItem, previousRecordingItem);

				//try to avoid flee and other actions
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actor.getActorPed(), true);
			}
		}
		else {
			return;
		}
	}

	//check for completion every recordingItem.getTicksDeltaCheckCompletion() ticks
	if (ticksNow >= recordingPlayback.getTicksLastCheckOfCurrentItem() + recordingItem->getTicksDeltaCheckCompletion()) {
		Vector3 currentLocation = ENTITY::GET_ENTITY_COORDS(actorPed, 1);
		log_to_file(std::to_string(ticksNow) + " checking for completion of item " + recordingItem->toString());

		if (recordingItem->isRecordingItemCompleted(nextRecordingItem, recordingPlayback.getTicksStartCurrentItem(), ticksNow, recordingPlayback.getAttemptsCheckedCompletion(), actor, currentLocation)) {
			//execute any post actions (normally empty)
			recordingItem->executeNativesAfterRecording(actor);

			//skip to next normally. But if last or ped is dead, skip to last
			if (recordingPlayback.isCurrentRecordedItemLast() || ENTITY::IS_ENTITY_DEAD(actorPed)) {
				recordingPlayback.setPlaybackCompleted();
				actor.stopReplayRecording();
			}
			else {
				recordingPlayback.nextRecordingItemIndex(GetTickCount());
				recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
				
				//skip disabled recording items
				while (recordingItem->isDisabled()) {
					log_to_file("DISABLED " + recordingItem->toString());
					if (recordingPlayback.isCurrentRecordedItemLast()) {
						recordingItem = nullptr;
						break;
					}
					else {
						recordingPlayback.nextRecordingItemIndex(GetTickCount());
						recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
					}
				}
				
				if (recordingItem != nullptr) {
					log_to_file("Starting next recorded item " + std::to_string(recordingPlayback.getRecordedItemIndex()) + " : " + recordingItem->toString());
					recordingItem->executeNativesForRecording(actor, nextRecordingItem, previousRecordingItem);
				}
				else {
					recordingPlayback.setPlaybackCompleted();
					actor.stopReplayRecording();
				}
			}
		}
		else {
			recordingPlayback.setTicksLastCheckOfCurrentItem(ticksNow);
			recordingPlayback.incrementAttempstCheckedCompletion();
		}
	}
}


ActorRecordingPlayback::ActorRecordingPlayback()
{
	m_recordingItemIndex = 0;
	m_ticksStartCurrentItem = 0;
	m_attemptsCheckCompletion = 0;
}

ActorRecordingPlayback::ActorRecordingPlayback(DWORD tickStart, int maxRecordingItemIndex)
{
	m_recordingItemIndex = 0;
	m_ticksStartCurrentItem = tickStart;
	m_ticksPlaybackStarted = tickStart;
	m_maxRecordingItemIndex = maxRecordingItemIndex;
	m_attemptsCheckCompletion = 0;
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
	m_recordingItemIndex = m_recordingItemIndex + 1;
	m_ticksLastCheckOfCurrentItem = ticksNow;
	m_ticksStartCurrentItem = ticksNow;
	m_attemptsCheckCompletion = 0;
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

int ActorRecordingPlayback::getAttemptsCheckedCompletion()
{
	return m_attemptsCheckCompletion;
}

void ActorRecordingPlayback::incrementAttempstCheckedCompletion()
{
	m_attemptsCheckCompletion++;
}

void ActorRecordingPlayback::setPlaybackCompleted()
{
	m_playbackCompleted = true;
	m_attemptsCheckCompletion = 0;
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


