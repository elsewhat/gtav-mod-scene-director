#include "actor.h"
#include <vector>
#include <algorithm>


Actor::Actor(Ped ped) {
	m_ped = ped;
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

void Actor::setHasSpotLight(bool hasSpotlight)
{
	m_hasSpotlight = hasSpotLight;
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

