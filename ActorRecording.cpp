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
	m_nrAttemptsBeforeSkipping = 25;
}

DWORD ActorRecordingItem::getTicksAfterRecordStart()
{
	return m_ticksAfterRecordStart;
}


Vector3 ActorRecordingItem::getLocation()
{
	return m_location;
}

void ActorRecordingItem::setLocation(Vector3 location)
{
	m_location = location;
}


int ActorRecordingItem::getNrAttemptsBeforeSkipping() {
	return m_nrAttemptsBeforeSkipping;
}

void ActorRecordingItem::setNrAttemptsBeforeSkipping(int nrAttemptsBeforeSkipping) {
	m_nrAttemptsBeforeSkipping = nrAttemptsBeforeSkipping;
}

void ActorRecordingItem::previewRecording(Actor* actor)
{
	//do nothing
}

void ActorRecordingItem::stopPreviewRecording(Actor* actor)
{
	//do nothing
}

void ActorRecordingItem::updatePreviewLocation(Actor* actor,Vector3 location)
{
	//do nothing
}



DWORD ActorRecordingItem::getTicksDeltaCheckCompletion()
{
	return m_ticksDeltaCheckCompletion;
}

void ActorRecordingItem::setTicksDeltaCheckCompletion(DWORD ticksDeltaCheckCompletion)
{
	m_ticksDeltaCheckCompletion = ticksDeltaCheckCompletion;
}

void ActorRecordingItem::setTicksLength(DWORD ticks)
{
	m_ticksLength = ticks;
}

DWORD ActorRecordingItem::getTicksLength()
{
	return m_ticksLength;
}

int ActorRecordingItem::getIndex()
{
	return m_index;
}

void ActorRecordingItem::setIndex(int index)
{
	m_index = index;
}

bool ActorRecordingItem::isDisabled()
{
	return m_isDisabled;
}

void ActorRecordingItem::setIsDisabled(bool isDisabled)
{
	m_isDisabled = isDisabled;
}

std::string ActorRecordingItem::toString()
{
	return "ActorRecordingItem " + std::to_string(m_actorPed) + " ticks: " + std::to_string(m_ticksAfterRecordStart) + " delta_ticks:" + std::to_string(m_ticksDeltaWhenRecorded);
}

void ActorRecordingItem::executeNativesAfterRecording(Actor actor)
{
}

void ActorRecordingItem::drawMarkerForRecording(bool isCurrent, bool showDisabled)
{
	int colorR = 204;
	int colorG = 204; 
	int colorB = 0;
	if (m_markerType == MARKER_TYPE_HIGHLIGHTED) {
		colorR = 135;
		colorG = 206;
		colorB = 250;
	}
	else if (m_markerType == MARKER_TYPE_SELECTED) {
		colorR = 255;
		colorG = 130;
		colorB = 130;
	}

	if (m_isDisabled) {
		if (showDisabled) {
			colorR = 255;
			colorG = 255;
			colorB = 255;
			GRAPHICS::DRAW_MARKER(1, m_location.x, m_location.y, m_location.z, 0.0f, 0.0f, 0.0f, 180.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, colorR, colorG, colorB, 100, false, true, 2, false, false, false, false);
		}
	}
	else {
		if (isCurrent) {
			GRAPHICS::DRAW_MARKER(2, m_location.x, m_location.y, m_location.z, 0.0f, 0.0f, 0.0f, 180.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, colorR, colorG, colorB, 100, false, true, 2, false, false, false, false);
		}
		else {
			GRAPHICS::DRAW_MARKER(1, m_location.x, m_location.y, m_location.z, 0.0f, 0.0f, 0.0f, 180.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, colorR, colorG, colorB, 100, false, true, 2, false, false, false, false);
		}
	}


}

void ActorRecordingItem::setMarkerType(MARKER_TYPE markerType)
{
	m_markerType = markerType;
}

MARKER_TYPE ActorRecordingItem::getMarkerType()
{
	return m_markerType;
}

std::string ActorRecordingItem::toUserFriendlyName()
{
	return "Recording";
}

void ActorRecordingItem::setMinDistanceBeforeCompleted(float minDistanceBeforeCompleted)
{
	m_minDistanceBeforeCompleted = minDistanceBeforeCompleted;
}

float ActorRecordingItem::getMinDistanceBeforeCompleted()
{
	return m_minDistanceBeforeCompleted;
}


ActorOnFootMovementRecordingItem::ActorOnFootMovementRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed, float headingAtEnd):ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_walkSpeed = walkSpeed;
	m_heading = headingAtEnd;

	//determine min distance before completion
	m_minDistanceBeforeCompleted = 4.0;
	if (m_walkSpeed > 1.0) {
		m_minDistanceBeforeCompleted = 7.0;
	}

}

float ActorOnFootMovementRecordingItem::getWalkSpeed()
{
	return m_walkSpeed;
}

void ActorOnFootMovementRecordingItem::setWalkSpeed(float walkSpeed)
{
	m_walkSpeed = walkSpeed;
}

void ActorOnFootMovementRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	AI::TASK_GO_STRAIGHT_TO_COORD(actor.getActorPed(), m_location.x, m_location.y, m_location.z, m_walkSpeed, -1, m_heading, 0.5f);

}

bool ActorOnFootMovementRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	if (ticksNow - ticksStart >= (m_ticksDeltaWhenRecorded / 2.0)) {
		float minDistance = getMinDistanceBeforeCompleted();
		

		//special handling if next recording is an animation or shooting
		if (nextRecordingItem) {
			std::shared_ptr<ActorAnimationSequenceRecordingItem> animationRecording = std::dynamic_pointer_cast<ActorAnimationSequenceRecordingItem>(nextRecordingItem);
			if (animationRecording) {
				log_to_file("Special handling since next recording is an ActorAnimationSequenceRecordingItem");
				minDistance = 0.5;
			}

			std::shared_ptr<ActorShootAtByImpactRecordingItem> shootingRecording = std::dynamic_pointer_cast<ActorShootAtByImpactRecordingItem>(nextRecordingItem);

			if (shootingRecording) {
				log_to_file("Special handling since next recording is an ActorShootAtByImpactRecordingItem");
				minDistance = 0.5;
			}
		}


		float distanceToTarget = SYSTEM::VDIST(m_location.x, m_location.y, m_location.z, location.x, location.y, location.z);
		log_to_file("ActorMovementRecordingItem: Distance to target: " + std::to_string(distanceToTarget) + " Min distance: " + std::to_string(minDistance));

		if (distanceToTarget < minDistance) {
			log_to_file("ActorMovementRecordingItem: Close enough to target for ActorRecordingItem");
			return true;
		}
		else {
			if (nrOfChecksForCompletion > m_nrAttemptsBeforeSkipping) {
				log_to_file("Giving up after " + std::to_string(nrOfChecksForCompletion) + " attempts");
				return true;
			}
			else {
				return false;
			}

		}
	}else {
	 log_to_file("Need to wait " + std::to_string((m_ticksDeltaWhenRecorded / 2.0)) + " before checking distance to target for completion");
	 return false;
 }
}


std::string ActorOnFootMovementRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorOnFootMovementRecordingItem Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ") Speed " + std::to_string(m_walkSpeed);
}

std::string ActorOnFootMovementRecordingItem::toUserFriendlyName()
{
	if (m_walkSpeed > 1.0) {
		return "Running";
	}
	else {
		return "Walking";
	}
}


ActorVehicleRecordingItem::ActorVehicleRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh,float vehHeading) :ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_vehicle = veh;
	m_vehicleType = _getVehicleTypeFromNatives();
	m_vehicleHeading = vehHeading;
}


Vehicle ActorVehicleRecordingItem::getVehicle()
{
	return m_vehicle;
}

float ActorVehicleRecordingItem::getVehicleHeading() {
	return m_vehicleHeading;
}

VEHICLE_TYPE ActorVehicleRecordingItem::getVehicleType()
{
	return m_vehicleType;
}


std::string ActorVehicleRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleRecordingItem Vehicle " + std::to_string(m_vehicle) + " Vehicle type " + std::to_string(m_vehicleType) + " Heading  " + std::to_string(m_vehicleHeading)+ " Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ")";
}

std::string ActorVehicleRecordingItem::toUserFriendlyName()
{
	return "Vehicle";
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



ActorVehicleEnterRecordingItem::ActorVehicleEnterRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading, int vehicleSeat, float enterVehicleSpeed) : ActorVehicleRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, veh, vehHeading)
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

void ActorVehicleEnterRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	AI::TASK_ENTER_VEHICLE(actor.getActorPed(), m_vehicle, -1, m_vehicleSeat, m_enterVehicleSpeed, 1, 0);
}

bool ActorVehicleEnterRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	if (PED::IS_PED_SITTING_IN_VEHICLE(actor.getActorPed(),m_vehicle)) {
		return true;
	}
	else {
		return false;
	}
}

std::string ActorVehicleEnterRecordingItem::toUserFriendlyName()
{
	return "Enter";
}

ActorVehicleExitRecordingItem::ActorVehicleExitRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading): ActorVehicleRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, veh, vehHeading)
{
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

std::string ActorVehicleExitRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleExitRecordingItem Vehicle " + std::to_string(m_vehicle);
}

void ActorVehicleExitRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	AI::TASK_LEAVE_VEHICLE(actor.getActorPed(), m_vehicle, 0);
}

bool ActorVehicleExitRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	if (PED::IS_PED_SITTING_IN_VEHICLE(actor.getActorPed(), m_vehicle)) {
		return false;
	}
	else {//extra check in order to make sure they are not in the vehicle
		if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0) && PED::GET_VEHICLE_PED_IS_USING(actor.getActorPed()) == m_vehicle) {
			return false;
		}
		else {
			return true;
		}
	}
}

std::string ActorVehicleExitRecordingItem::toUserFriendlyName()
{
	return "Exit";
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

void ActorStandingStillRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	AI::CLEAR_PED_TASKS(actor.getActorPed());
}

bool ActorStandingStillRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart,  DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	//will first be checked after m_ticksDeltaCheckCompletion
	return true;
}

std::string ActorStandingStillRecordingItem::toUserFriendlyName()
{
	return "Waiting";
}

ActorVehicleMovementRecordingItem::ActorVehicleMovementRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading, float speedInVehicle) : ActorVehicleRecordingItem(ticksStart,  ticksDeltaWhenRecorded, actor, location, veh, vehHeading)
{
	m_speedInVehicle = speedInVehicle;


	Hash vehicleHash = ENTITY::GET_ENTITY_MODEL(veh);

	bool isInVehicle = PED::IS_PED_IN_ANY_VEHICLE(actor, 0);
	bool isPedInHeli = VEHICLE::IS_THIS_MODEL_A_HELI(vehicleHash);
	bool isPedInPlane = VEHICLE::IS_THIS_MODEL_A_PLANE(vehicleHash);
	bool isPedInBoat = VEHICLE::IS_THIS_MODEL_A_BOAT(vehicleHash);

	m_minDistanceBeforeCompleted = 4.0;

	if (isPedInHeli) {
		m_minDistanceBeforeCompleted = 90.0;
		m_nrAttemptsBeforeSkipping = 100;
	}
	else if (isPedInPlane) {
		m_minDistanceBeforeCompleted = 100.0;
	}
	else if (isPedInBoat) {
		m_minDistanceBeforeCompleted = 60.0;
	}
	else if (isInVehicle) {
		//TODO: Tweak based on heading changes
		if (m_speedInVehicle > 30.0) {
			m_minDistanceBeforeCompleted = 17.0;
		}
		else {
			m_minDistanceBeforeCompleted = 10.0;
		}
	}

}

std::string ActorVehicleMovementRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleMovementRecordingItem Vehicle " + std::to_string(m_vehicle) + " speed : " + std::to_string(m_speedInVehicle);
}

float ActorVehicleMovementRecordingItem::getSpeedInVehicle()
{
	return m_speedInVehicle;
}

void ActorVehicleMovementRecordingItem::setSpeedInVehicle(float speedInVehicle)
{
	m_speedInVehicle = speedInVehicle;
}

void ActorVehicleMovementRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actor.getActorPed());

		//we assume pedVehicle is the right vehicle

		//check if player is the driver
		Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
		if (pedDriver != actor.getActorPed()) {
			log_to_file("ActorVehicleMovementRecordingItem: Actor (" + std::to_string(actor.getActorPed()) + " is not driver (" + std::to_string(pedDriver) + ") Will do no action");
			//set_status_text("Ped is not driver. Ignore waypoint");
		}
		else {

			if (PED::IS_PED_IN_ANY_HELI(actor.getActorPed())) {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, m_location.x, m_location.y, m_location.z, m_speedInVehicle, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, -1.0, -1);
				log_to_file("playback_recording_to_waypoint: Flying in heli with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));
			}
			else if (PED::IS_PED_IN_ANY_PLANE(actor.getActorPed())) {
				AI::TASK_PLANE_MISSION(pedDriver, pedVehicle, 0, 0, m_location.x, m_location.y, m_location.z, 4, 30.0, 50.0, -1, m_speedInVehicle, 50);
				log_to_file("playback_recording_to_waypoint: Flying in plane with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));

			}
			else if (PED::IS_PED_IN_ANY_BOAT(actor.getActorPed())) {
				AI::TASK_BOAT_MISSION(pedDriver, pedVehicle, 0, 0, m_location.x, m_location.y, m_location.z, 4, m_speedInVehicle, 786469, -1.0, 7);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(pedDriver, 1);
				//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, 0.0, 20.0, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786469, 5.0, 1071);
				log_to_file("playback_recording_to_waypoint: In boat : " + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));
			}
			else {
				//adjust speed based on previous and next recording item
				std::shared_ptr<ActorVehicleMovementRecordingItem> nextVehicleMovementRecording = std::dynamic_pointer_cast<ActorVehicleMovementRecordingItem>(nextRecordingItem);
				std::shared_ptr<ActorVehicleMovementRecordingItem> prevVehicleMovementRecording = std::dynamic_pointer_cast<ActorVehicleMovementRecordingItem>(previousRecordingItem);

				float speedForPlayback = m_speedInVehicle;
				//only take action if both previous and next recordings are in car
				if (nextVehicleMovementRecording && prevVehicleMovementRecording) {
					float nextHeading = nextVehicleMovementRecording->getVehicleHeading();
					float previousHeading = prevVehicleMovementRecording->getVehicleHeading();

					log_to_file("Heading " + std::to_string(m_vehicleHeading) + " prev:" + std::to_string(m_vehicleHeading) + " next" + std::to_string(previousHeading));
				}
				
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, m_location.x, m_location.y, m_location.z, speedForPlayback, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), actor.getDrivingMode().value, 2.0, -1);
				log_to_file("playback_recording_to_waypoint: Driving with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(m_speedInVehicle));

			}
		}

	}

}

bool ActorVehicleMovementRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	if (ticksNow - ticksStart >= (m_ticksDeltaWhenRecorded - 500)) {
		//log_to_file("ticksNow - ticksStart = " + std::to_string(ticksNow - ticksStart) + " (m_ticksDeltaCheckCompletion / 2.0) = " + std::to_string((m_ticksDeltaWhenRecorded / 2.0)));
		if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actor.getActorPed());

			bool isInVehicle = PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0);
			bool isPedInHeli = PED::IS_PED_IN_ANY_HELI(actor.getActorPed());
			bool isPedInPlane = PED::IS_PED_IN_ANY_PLANE(actor.getActorPed());
			bool isPedInBoat = PED::IS_PED_IN_ANY_BOAT(actor.getActorPed());

			float minDistance = getMinDistanceBeforeCompleted();


			//check if next is not a vehicle movement (will often be exit vehicle) Then the threshold should be much less
			std::shared_ptr<ActorVehicleMovementRecordingItem> nextVehicleMovement = std::dynamic_pointer_cast<ActorVehicleMovementRecordingItem>(nextRecordingItem);
			log_to_file("nextVehicleMovement: ");

			//&& nextVehicleRocketBoost == NULL
			if (nextVehicleMovement == NULL) {
				log_to_file("Next recording is not a vehicle movement, will require a smaller distance to target");
				if (isPedInHeli && minDistance>45.0) {
					minDistance = 45.0;
				}
				else if (isPedInPlane && minDistance>50.0) {
					minDistance = 50.0;
				}
				else if (isPedInBoat && minDistance>30.0) {
					minDistance = 30.0;
				}
				else if (isInVehicle && minDistance>2.0) {
					minDistance = 2.0;
				}
			}
			else {
				if (nextVehicleMovement->getSpeedInVehicle() == 0.0) {
					log_to_file("nextVehicleMovement->getSpeedInVehicle() == 0");
					if (isInVehicle && minDistance>2.0) {
						minDistance = 2.0;
					}
				}

			}


			float distanceToTarget = SYSTEM::VDIST(m_location.x, m_location.y, m_location.z, location.x, location.y, location.z);
			log_to_file("ActorVehicleMovementRecordingItem: Distance to target: " + std::to_string(distanceToTarget));

			if (distanceToTarget < minDistance) {
				log_to_file("ActorVehicleMovementRecordingItem: Close enough to target for ActorRecordingItem");
				return true;
			}
			else {
				if (nrOfChecksForCompletion > m_nrAttemptsBeforeSkipping) {
					log_to_file("Giving up after " + std::to_string(nrOfChecksForCompletion) + " attempts");
					return true;
				}
				else {
					return false;
				}
			}

		}
		else {
			//not in any vehicle so return true
			log_to_file("ActorVehicleMovementRecordingItem: Not in any vehicle so returning true isRecordingItemCompleted");
			return true;
		}
	} else {
		log_to_file("Need to wait " + std::to_string((m_ticksDeltaWhenRecorded / 2.0)) + " before checking distance to target for completion");
		return false;
	}
}

std::string ActorVehicleMovementRecordingItem::toUserFriendlyName()
{
	return "Vehicle";
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

void ActorScenarioRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	if (m_scenario.hasEnterAnim) {
		AI::TASK_START_SCENARIO_IN_PLACE(actor.getActorPed(), m_scenario.name, -1, 1);
	}
	else {
		AI::TASK_START_SCENARIO_IN_PLACE(actor.getActorPed(), m_scenario.name, -1, 0);
	}
}

bool ActorScenarioRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
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
	AI::CLEAR_PED_TASKS(actor.getActorPed());
}

std::string ActorScenarioRecordingItem::toUserFriendlyName()
{
	return "Scenario";
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

void ActorAimAtRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	AI::TASK_AIM_GUN_AT_ENTITY(actor.getActorPed(), m_aimedAtEntity, -1, 0);
}

bool ActorAimAtRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
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

std::string ActorAimAtRecordingItem::toUserFriendlyName()
{
	return "Aim";
}

ActorShootAtEntityRecordingItem::ActorShootAtEntityRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Entity shotAtEntity) :ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor,location)
{
	m_shotAtEntity = shotAtEntity;
	//check for completion every 200 ticks (default 1000)
	m_ticksDeltaCheckCompletion = 100;
}

Entity ActorShootAtEntityRecordingItem::getShotAtEntity()
{
	return m_shotAtEntity;
}

std::string ActorShootAtEntityRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorShootAtEntityRecordingItem Entity " + std::to_string(m_shotAtEntity);
}

void ActorShootAtEntityRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	AI::TASK_SHOOT_AT_ENTITY(actor.getActorPed(), m_shotAtEntity, -1, GAMEPLAY::GET_HASH_KEY("FIRING_PATTERN_SINGLE_SHOT"));
}

bool ActorShootAtEntityRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	return true;
}

void ActorShootAtEntityRecordingItem::executeNativesAfterRecording(Actor actor)
{
	AI::CLEAR_PED_TASKS(actor.getActorPed());
}

std::string ActorShootAtEntityRecordingItem::toUserFriendlyName()
{
	return "Shoot";
}

ActorAnimationSequenceRecordingItem::ActorAnimationSequenceRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float heading, AnimationSequence animationSequence, AnimationFlag animationFlag):ActorRecordingItem(ticksStart,ticksDeltaWhenRecorded,actor,location)
{
	m_animationSequence = animationSequence;
	m_animationFlag = animationFlag;
	m_ticksDeltaCheckCompletion = 0;
	m_heading = heading;
}

AnimationSequence ActorAnimationSequenceRecordingItem::getAnimationSequence()
{
	return m_animationSequence;
}

void ActorAnimationSequenceRecordingItem::setAnimationSequence(AnimationSequence animationSequence)
{
	m_animationSequence = animationSequence;
}

std::string ActorAnimationSequenceRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorAnimationSequenceRecordingItem: " + m_animationSequence.toString();
}

void ActorAnimationSequenceRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	//log_to_file("START ActorAnimationSequenceRecordingItem::executeNativesForRecording " + std::to_string(GetTickCount()));
	//load animation dicts
	for (auto &animation : m_animationSequence.animationsInSequence) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);

		DWORD ticksStart = GetTickCount();

		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() > ticksStart + 5000) {
				//duration will be 0 if it's not loaded
				log_to_file("Ticks overflow2");
				set_status_text("Could not load animation with code " + std::string(animation.animLibrary));
				return;
			}
		}
	}

	TaskSequence task_seq = 1;
	AI::OPEN_SEQUENCE_TASK(&task_seq);

	//load animation dicts
	for (auto &animation : m_animationSequence.animationsInSequence) {
		AI::TASK_PLAY_ANIM(0, animation.animLibrary, animation.animName, 8.0f, -8.0f, animation.duration, m_animationFlag.id, 8.0f, 0, 0, 0);
	}

	AI::CLOSE_SEQUENCE_TASK(task_seq);
	AI::TASK_PERFORM_SEQUENCE(actor.getActorPed(), task_seq);
	AI::CLEAR_SEQUENCE_TASK(&task_seq);

	//log_to_file("END ActorAnimationSequenceRecordingItem::executeNativesForRecording " + std::to_string(GetTickCount()));

}

bool ActorAnimationSequenceRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	int progress = AI::GET_SEQUENCE_PROGRESS(actor.getActorPed());
	
	log_to_file("ActorAnimationSequenceRecordingItem::isRecordingItemCompleted " + std::to_string(progress));
	return true;
}

std::string ActorAnimationSequenceRecordingItem::toUserFriendlyName()
{
	return "Animation";
}

void ActorAnimationSequenceRecordingItem::previewRecording(Actor * actor)
{
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(actor->getActorPed(), m_location.x, m_location.y, m_location.z, 0, 0, 1);
	ENTITY::SET_ENTITY_HEADING(actor->getActorPed(), m_heading);
	WAIT(100);

	//load animation dicts
	for (auto &animation : m_animationSequence.animationsInSequence) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);

		DWORD ticksStart = GetTickCount();

		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() > ticksStart + 5000) {
				//duration will be 0 if it's not loaded
				log_to_file("Ticks overflow2");
				set_status_text("Could not load animation with code " + std::string(animation.animLibrary));
				return;
			}
		}
	}

	TaskSequence task_seq = 1;
	AI::OPEN_SEQUENCE_TASK(&task_seq);

	//load animation dicts
	for (auto &animation : m_animationSequence.animationsInSequence) {
		AI::TASK_PLAY_ANIM(0, animation.animLibrary, animation.animName, 8.0f, -8.0f, animation.duration, m_animationFlag.id, 8.0f, 0, 0, 0);
	}
	AI::TASK_PED_SLIDE_TO_COORD(0, m_location.x, m_location.y, m_location.z, m_heading, 1061158912);
	AI::SET_SEQUENCE_TO_REPEAT(task_seq, true);

	AI::CLOSE_SEQUENCE_TASK(task_seq);
	AI::TASK_PERFORM_SEQUENCE(actor->getActorPed(), task_seq);
	AI::CLEAR_SEQUENCE_TASK(&task_seq);
}

void ActorAnimationSequenceRecordingItem::stopPreviewRecording(Actor* actor)
{
	AI::CLEAR_PED_TASKS(actor->getActorPed());
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(actor->getActorPed(), m_location.x, m_location.y, m_location.z, 0, 0, 1);
	ENTITY::SET_ENTITY_HEADING(actor->getActorPed(), m_heading);
}

void ActorAnimationSequenceRecordingItem::updatePreviewLocation(Actor* actor, Vector3 location)
{
	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(actor->getActorPed(), location.x, location.y, location.z, 0, 0, 1);
	ENTITY::SET_ENTITY_HEADING(actor->getActorPed(), m_heading);
	previewRecording(actor);
}

float ActorAnimationSequenceRecordingItem::getHeading()
{
	return m_heading;
}

void ActorAnimationSequenceRecordingItem::setHeading(float heading)
{
	m_heading = heading;
}


ActorCoverAtRecordingItem::ActorCoverAtRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vector3 enterCoverPosition, Vector3 coverPosition) : ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_coverPosition = coverPosition;
	m_enterCoverPosition = enterCoverPosition;

	//delta check every 100ms (1000 is default)
	m_ticksDeltaCheckCompletion = 100;
}

void ActorCoverAtRecordingItem::setCoverPosition(Vector3 coverPosition)
{
	m_coverPosition = coverPosition;
}

std::string ActorCoverAtRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorCoverAtRecordingItem location (" + std::to_string(m_coverPosition.x)+","+ std::to_string(m_coverPosition.y)+","+ std::to_string(m_coverPosition.z) +") Length: "+ std::to_string(m_ticksLength);
}

void ActorCoverAtRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	//AI::TASK_SEEK_COVER_FROM_POS (actor.getActorPed(), m_coverPosition.x,m_coverPosition.y,m_coverPosition.z, 5000, 1);
	Vector3 currentLocation = ENTITY::GET_ENTITY_COORDS(actor.getActorPed(), true);
	//AI::TASK_SEEK_COVER_TO_COORDS(actor.getActorPed(), currentLocation.x, currentLocation.y, currentLocation.z, m_enterCoverPosition.x, m_enterCoverPosition.y, m_enterCoverPosition.z,  -1, 0);
	AI::TASK_PUT_PED_DIRECTLY_INTO_COVER(actor.getActorPed(), m_enterCoverPosition.x, m_enterCoverPosition.y, m_enterCoverPosition.z, -1, 0, 0.0, 1, 1, 0, 0);
}

bool ActorCoverAtRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	if (PED::IS_PED_IN_COVER(actor.getActorPed(), 1)) {
		return true;
	}else if (ticksNow - ticksStart >= 5000) {
		return true;
	}
	else {
		return false;
	}
}

std::string ActorCoverAtRecordingItem::toUserFriendlyName()
{
	return "Cover";
}

ActorShootAtByImpactRecordingItem::ActorShootAtByImpactRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Hash weapon, Vector3 weaponImpact, Hash firingPattern, float walkSpeed, float heading):ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_weapon = weapon;
	m_weaponImpact = weaponImpact;
	m_firingPattern = firingPattern;
	m_walkSpeed = walkSpeed;
	m_heading = heading;

	m_ticksDeltaCheckCompletion = ticksDeltaWhenRecorded;
}

std::string ActorShootAtByImpactRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorShootAtByImpactRecordingItem location (" + std::to_string(m_weaponImpact.x) + "," + std::to_string(m_weaponImpact.y) + "," + std::to_string(m_weaponImpact.z) + ")" + " walkspeed " + std::to_string(m_walkSpeed) + " heading " + std::to_string(m_heading);
}

void ActorShootAtByImpactRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	//disable "INPUTGROUP_LOOK" = 1
	//CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(1);
	//CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);

	//load weapon
	if (!WEAPON::HAS_WEAPON_ASSET_LOADED(m_weapon))
	{
		WEAPON::REQUEST_WEAPON_ASSET(m_weapon, 31, 0);
		while (!WEAPON::HAS_WEAPON_ASSET_LOADED(m_weapon))
			WAIT(0);
	}

	//check the weapon of the actor
	Hash currentWeapon;
	WEAPON::GET_CURRENT_PED_WEAPON(actor.getActorPed(), &currentWeapon, 1);
	if (currentWeapon != m_weapon) {
		WEAPON::GIVE_WEAPON_TO_PED(actor.getActorPed(), m_weapon, 1000, 1, 1);
	}

	Vector3 currentLocation = ENTITY::GET_ENTITY_COORDS(actor.getActorPed(), true);
	//GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(currentLocation.x, currentLocation.y, currentLocation.z, m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 100, 1, m_weapon, actor.getActorPed(), 1, 1, 100.0);
	

	if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
		//WEAPON::SET_CURRENT_PED_VEHICLE_WEAPON(actor.getActorPed(), m_weapon);
		//WEAPON::SET_CURRENT_PED_WEAPON(actor.getActorPed(), m_weapon, 1);
		PED::SET_PED_SHOOTS_AT_COORD(actor.getActorPed(), m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 1);
		AI::TASK_AIM_GUN_AT_COORD(actor.getActorPed(), m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 1000, 1, 1);
	}
	else {

		if (m_walkSpeed > 0) {
			AI::TASK_GO_TO_COORD_WHILE_AIMING_AT_COORD(actor.getActorPed(), m_location.x, m_location.y, m_location.z, m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 1, false, m_walkSpeed, m_heading, true, 0, 0, m_firingPattern);
		}
		else {
			AI::TASK_AIM_GUN_AT_COORD(actor.getActorPed(), m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 1000, 0, 0);
			PED::SET_PED_DESIRED_HEADING(actor.getActorPed(), m_heading);
		}

		PED::SET_PED_SHOOTS_AT_COORD(actor.getActorPed(), m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 1);
	}


}

bool ActorShootAtByImpactRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{

	//if walkspeed is zero, recording is complete immediately. If not perform calculation on actual location
	if (m_walkSpeed <= 0) {
		return true;
	}else if (ticksNow - ticksStart >= (m_ticksDeltaWhenRecorded / 2.0)) {
		float minDistance = 4.0;
		if (m_walkSpeed > 1.0) {
			minDistance = 7.0;
		}

		//special handling if next recording is an animation
		if (nextRecordingItem) {
			std::shared_ptr<ActorAnimationSequenceRecordingItem> animationRecording = std::dynamic_pointer_cast<ActorAnimationSequenceRecordingItem>(nextRecordingItem);

			if (animationRecording) {
				log_to_file("Special handling since next recording is an animation");
				minDistance = 0.5;
			}
		}


		float distanceToTarget = SYSTEM::VDIST(m_location.x, m_location.y, m_location.z, location.x, location.y, location.z);
		log_to_file("ActorShootAtByImpactRecordingItem: Distance to target: " + std::to_string(distanceToTarget) + " Min distance: " + std::to_string(minDistance));

		if (distanceToTarget < minDistance) {
			log_to_file("ActorShootAtByImpactRecordingItem: Close enough to target for ActorRecordingItem");
			return true;
		}
		else {
			if (nrOfChecksForCompletion > 10) {
				log_to_file("Giving up after " + std::to_string(nrOfChecksForCompletion) + " attempts");
				return true;
			}
			else {
				return false;
			}

		}
	}
	else {
		log_to_file("Need to wait " + std::to_string((m_ticksDeltaWhenRecorded / 2.0)) + " before checking distance to target for completion");
		return false;
	}
}

void ActorShootAtByImpactRecordingItem::executeNativesAfterRecording(Actor actor)
{
	//log_to_file("ActorShootAtByImpactRecordingItem: executeNativesAfterRecording calling AI::CLEAR_PED_TASKS");
	//AI::CLEAR_PED_TASKS(actor.getActorPed());
}

std::string ActorShootAtByImpactRecordingItem::toUserFriendlyName()
{
	return "Shoot";
}

ActorJumpingRecordingItem::ActorJumpingRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed, float headingAtEnd, bool isClimbing):ActorOnFootMovementRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, walkSpeed, headingAtEnd)
{
	//TODO
	m_isClimbing = isClimbing;
	
}



void ActorJumpingRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	if (m_isClimbing) {
		AI::TASK_CLIMB(actor.getActorPed(), 0);
	}
	else {
		AI::TASK_JUMP(actor.getActorPed(), 0);
	}
	
}

bool ActorJumpingRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	return true;
}


std::string ActorJumpingRecordingItem::toString()
{
	return "ActorJumpingRecordingItem Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ") isClimbing:"+std::to_string(m_isClimbing)+" Speed:" + std::to_string(m_walkSpeed);
}

std::string ActorJumpingRecordingItem::toUserFriendlyName()
{
	return "Jumping";
}

bool ActorJumpingRecordingItem::isClimbing()
{
	return m_isClimbing;
}

void ActorJumpingRecordingItem::setIsClimbing(bool isClimbing)
{
	m_isClimbing = isClimbing;
}

ActorReloadRecordingItem::ActorReloadRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Hash weapon, bool doAim, Vector3 weaponImpact):ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location)
{
	m_weapon = weapon;
	m_doAim = doAim;
	m_weaponImpact = weaponImpact;
	setTicksDeltaCheckCompletion(50);
}

void ActorReloadRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{

	AI::TASK_RELOAD_WEAPON(actor.getActorPed(), 1);
}

bool ActorReloadRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	//hard check on 1500 ticks as natives does not return false quickly enough
	if (PED::IS_PED_RELOADING(actor.getActorPed()) && ticksNow-ticksStart<1200) {
		return false;
	}
	else {
		//could not get task sequence to work with reload, so do this here
		if (m_doAim) {
			AI::TASK_AIM_GUN_AT_COORD(actor.getActorPed(), m_weaponImpact.x, m_weaponImpact.y, m_weaponImpact.z, 1000, 0, 0);
		}
		log_to_file("Used " + std::to_string(ticksNow - ticksStart) + " ticks to reload");
		return true;
	}
}

std::string ActorReloadRecordingItem::toUserFriendlyName()
{
	return "Reload";
}


std::string ActorReloadRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorReloadRecordingItem ";
}

ActorSpeakRecordingItem::ActorSpeakRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, float walkSpeed, float headingAtEnd, bool isMovingWhileSpeaking):ActorOnFootMovementRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, walkSpeed, headingAtEnd)
{
	m_isMovingWhileSpeaking = isMovingWhileSpeaking;
	setTicksDeltaCheckCompletion(50);
}
void ActorSpeakRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem) {
	STREAMING::REQUEST_ANIM_DICT("mp_facial");
	while (!STREAMING::HAS_ANIM_DICT_LOADED("mp_facial"))
	{
		WAIT(0);
	}

	//Animation flags are type "Facial"
	AI::TASK_PLAY_ANIM(actor.getActorPed(), "mp_facial", "mic_chatter", 8.0f, -8.0f, -1, ANIMATION_LOOP_FLAG1 | ANIMATION_ALLOW_MOVEMENT_FLAG6, 8.0f, 0, 0, 0);

	if (m_isMovingWhileSpeaking) {
		AI::TASK_GO_STRAIGHT_TO_COORD(actor.getActorPed(), m_location.x, m_location.y, m_location.z, m_walkSpeed, -1, m_heading, 0.5f);
	}
}
bool ActorSpeakRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location) {
	if ((ticksNow - ticksStart) > getTicksLength()) {
		return true;
	}
	else {
		return false;
	}
}
std::string ActorSpeakRecordingItem::toString() {
	return "ActorSpeakRecordingItem Length:" + std::to_string(getTicksLength()) +" "+ ActorOnFootMovementRecordingItem::toString();
}

void ActorSpeakRecordingItem::executeNativesAfterRecording(Actor actor)
{
	AI::STOP_ANIM_TASK(actor.getActorPed(), "mp_facial", "mic_chatter", -4.0);
}

std::string ActorSpeakRecordingItem::toUserFriendlyName()
{
	return "Speaking";
}

ActorSyncedAnimationRecordingItem::ActorSyncedAnimationRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded,Ped pedActor, std::vector<Actor*> actors, Vector3 location, float actorRotation, SyncedAnimation *syncedAnimation):ActorRecordingItem(ticksStart, ticksDeltaWhenRecorded, pedActor, location)
{
	m_syncedAnimation = syncedAnimation;
	m_actors = actors;
	m_ticksDeltaCheckCompletion = 0;
	m_rotation = actorRotation;
}

SyncedAnimation* ActorSyncedAnimationRecordingItem::getSyncedAnimation()
{
	return m_syncedAnimation;
}

void ActorSyncedAnimationRecordingItem::setSyncedAnimation(SyncedAnimation* syncedAnimation)
{
	m_syncedAnimation = syncedAnimation;
}

std::vector<Actor*> ActorSyncedAnimationRecordingItem::getActors()
{
	return m_actors;
}

void ActorSyncedAnimationRecordingItem::setActors(std::vector<Actor*> actors)
{
	m_actors = actors;
}

std::string ActorSyncedAnimationRecordingItem::toString()
{
	return "ActorSyncedAnimationRecordingItem: " + m_syncedAnimation->toString();
}

void ActorSyncedAnimationRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	log_to_file("ActorSyncedAnimationRecordingItem::executeNativesForRecording");
	m_syncedAnimation->executeSyncedAnimation(true,m_actors, m_useActorLocation, m_location, m_doLooping, m_useActorRotation,m_rotation);


}

bool ActorSyncedAnimationRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	if (m_syncedAnimation->isCompleted()) {
		return true;
	}
	else {
		return false;
	}
}

void ActorSyncedAnimationRecordingItem::executeNativesAfterRecording(Actor actor)
{
	m_syncedAnimation->cleanupAfterExecution(!m_keepProps, false);
}

std::string ActorSyncedAnimationRecordingItem::toUserFriendlyName()
{
	return "Anim";
}

void ActorSyncedAnimationRecordingItem::setDoLooping(bool doLooping)
{
	m_doLooping = doLooping;
	if (m_doLooping) {
		m_nrAttemptsBeforeSkipping = 10000;
		m_ticksDeltaCheckCompletion = 10000;
	}
	else {
		m_nrAttemptsBeforeSkipping = 1000;
		m_nrAttemptsBeforeSkipping = 100;
	}
}

bool ActorSyncedAnimationRecordingItem::getDoLooping()
{
	return m_doLooping;
}

void ActorSyncedAnimationRecordingItem::setLocation(Vector3 location)
{
	ActorRecordingItem::setLocation(location);

}

void ActorSyncedAnimationRecordingItem::setKeepProps(bool keepProps)
{
	m_keepProps = keepProps;
}

bool ActorSyncedAnimationRecordingItem::getKeepProps()
{
	return m_keepProps;
}

void ActorSyncedAnimationRecordingItem::setUseActorLocation(bool useActorLocation)
{
	m_useActorLocation = useActorLocation;
}

bool ActorSyncedAnimationRecordingItem::getUseActorLocation()
{
	return m_useActorLocation;
}

void ActorSyncedAnimationRecordingItem::setRotation(float rotation)
{
	m_rotation = rotation;
}

float ActorSyncedAnimationRecordingItem::getRotation()
{
	return m_rotation;
}

void ActorSyncedAnimationRecordingItem::setUseActorRotation(bool useActorRotation)
{
	m_useActorLocation = useActorRotation;
}

void ActorSyncedAnimationRecordingItem::previewRecording(Actor* actor)
{
	log_to_file("ActorSyncedAnimationRecordingItem::previewRecording");
	m_syncedAnimation->previewSyncedAnimation(m_actors, false, m_location, m_doLooping, false, m_rotation);
}

void ActorSyncedAnimationRecordingItem::stopPreviewRecording(Actor* actor)
{
	m_syncedAnimation->cleanupAfterExecution(true, true);
}

void ActorSyncedAnimationRecordingItem::updatePreviewLocation(Actor* actor,Vector3 location)
{
	log_to_file("ActorSyncedAnimationRecordingItem::setPreviewLocation");
	//m_syncedAnimation->updateLocationOfScene(location);
	if (m_syncedAnimation->isActive()) {
		//m_syncedAnimation->updateLocationOfScene(location);
		stopPreviewRecording(actor);
		previewRecording(actor);

	}
}

ActorVehicleRocketBoostRecordingItem::ActorVehicleRocketBoostRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading):ActorVehicleMovementRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, veh, vehHeading, 30.0f)
{
	m_ticksDeltaCheckCompletion = 10;
}

std::string ActorVehicleRocketBoostRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleRocketBoostRecordingItem Vehicle " + std::to_string(m_vehicle) ;
}

void ActorVehicleRocketBoostRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	if (VEHICLE::_HAS_VEHICLE_ROCKET_BOOST(m_vehicle)) {
		VEHICLE::_SET_VEHICLE_ROCKET_BOOST_ACTIVE(m_vehicle, true);
	}
	else {
		log_to_file("Vehicle has no rocket boost");
	}
}

bool ActorVehicleRocketBoostRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	return true;
}

std::string ActorVehicleRocketBoostRecordingItem::toUserFriendlyName()
{
	return "Boost";
}


ActorVehicleParachuteRecordingItem::ActorVehicleParachuteRecordingItem(DWORD ticksStart, DWORD ticksDeltaWhenRecorded, Ped actor, Vector3 location, Vehicle veh, float vehHeading) :ActorVehicleMovementRecordingItem(ticksStart, ticksDeltaWhenRecorded, actor, location, veh, vehHeading, 30.0f)
{
	m_ticksDeltaCheckCompletion = 10;
}

std::string ActorVehicleParachuteRecordingItem::toString()
{
	return ActorRecordingItem::toString() + " ActorVehicleParachuteRecordingItem Vehicle " + std::to_string(m_vehicle);
}

void ActorVehicleParachuteRecordingItem::executeNativesForRecording(Actor actor, std::shared_ptr<ActorRecordingItem> nextRecordingItem, std::shared_ptr<ActorRecordingItem> previousRecordingItem)
{
	if (VEHICLE::_HAS_VEHICLE_PARACHUTE(m_vehicle)) {
		VEHICLE::_SET_VEHICLE_PARACHUTE_ACTIVE(m_vehicle, true);
	}
	else {
		log_to_file("Vehicle has no parachute");
	}
}

bool ActorVehicleParachuteRecordingItem::isRecordingItemCompleted(std::shared_ptr<ActorRecordingItem> nextRecordingItem, DWORD ticksStart, DWORD ticksNow, int nrOfChecksForCompletion, Actor actor, Vector3 location)
{
	return true;
}

std::string ActorVehicleParachuteRecordingItem::toUserFriendlyName()
{
	return "Parachute";
}