#include <vector>
#include <algorithm>
#include "ActorRecording.h"

ActorRecordingItem::ActorRecordingItem(DWORD ticksStart, Ped actor)
{
	m_ticksAfterRecordStart = ticksStart;
	m_actor = actor;
}

DWORD ActorRecordingItem::getTicksAfterRecordStart()
{
	return m_ticksAfterRecordStart;
}

std::string ActorRecordingItem::to_string()
{
	return "Recording start for actor " + std::to_string(m_actor) + " at " + std::to_string(m_ticksAfterRecordStart);
}

ActorMovementRecordingItem::ActorMovementRecordingItem(DWORD ticksStart, Ped actor, Vector3 location, float heading):ActorRecordingItem(ticksStart, actor)
{
	m_location = location;
	m_heading = heading;

}

Vector3 ActorMovementRecordingItem::getLocation()
{
	return m_location;
}

float ActorMovementRecordingItem::getHeading()
{
	return m_heading;
}

std::string ActorMovementRecordingItem::to_string()
{
	return ActorRecordingItem::to_string() + " ActorMovementRecordingItem Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ") Heading " + std::to_string(m_heading);
}


ActorVehicleRecordingItem::ActorVehicleRecordingItem(DWORD ticksStart, Ped actor, Vehicle veh, Vector3 location) :ActorRecordingItem(ticksStart, actor)
{
	m_vehicle = veh;
	m_location = location;
	m_vehicleType = _getVehicleTypeFromNatives();
}

Vector3 ActorVehicleRecordingItem::getLocation()
{
	return m_location;
}

Vehicle ActorVehicleRecordingItem::getVehicle()
{
	return m_vehicle;
}

VEHICLE_TYPE ActorVehicleRecordingItem::getVehicleType()
{
	return m_vehicleType;
}

std::string ActorVehicleRecordingItem::to_string()
{
	return ActorRecordingItem::to_string() + " ActorVehicleRecordingItem Vehicle " + std::to_string(m_vehicle) + " Vehicle type " + std::to_string(m_vehicleType) + " Location (" + std::to_string(m_location.x) + "," + std::to_string(m_location.y) + "," + std::to_string(m_location.z) + ")";
}

VEHICLE_TYPE ActorVehicleRecordingItem::_getVehicleTypeFromNatives()
{
	//natives for VEHICLE all require the Hash of the model, so checking actor instead
	if (PED::IS_PED_IN_ANY_HELI(m_actor)) {
		return VEHICLE_TYPE_HELI;
	}
	else if (PED::IS_PED_IN_ANY_PLANE(m_actor)) {
		return VEHICLE_TYPE_PLANE;
	}
	else if (PED::IS_PED_IN_ANY_BOAT(m_actor)) {
		return VEHICLE_TYPE_BOAT;
	}
	else if (PED::IS_PED_IN_ANY_SUB(m_actor)) {
		return VEHICLE_TYPE_SUB;
	}
	else if (PED::IS_PED_ON_ANY_BIKE(m_actor)){
		return VEHICLE_TYPE_BIKE;
	}
	else {
		return VEHICLE_TYPE_CAR;
	}
}

ActorRecordingPlayback::ActorRecordingPlayback()
{
	m_recordingItemIndex = 0;
	m_tickStartCurrentItem = 0;
}

ActorRecordingPlayback::ActorRecordingPlayback(DWORD tickStart)
{
	m_recordingItemIndex = 0;
	m_tickStartCurrentItem = tickStart;
}

DWORD ActorRecordingPlayback::getTicksAfterRecordStart()
{
	return m_tickStartCurrentItem;
}

void ActorRecordingPlayback::setRecordingItemIndex(int index)
{
	m_recordingItemIndex = index;
}

int ActorRecordingPlayback::getRecordingItemIndex()
{
	return m_recordingItemIndex;
}
