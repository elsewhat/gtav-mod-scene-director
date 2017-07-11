#pragma once
#include <vector>
#include "GTAObject.h"



class StageLightFlicker {
public:
	bool isOn;
	DWORD length;
};

class StageLightRotation {
public:
	float pitch;
	float yaw;
	bool hasPitch;
	bool hasYaw;
	DWORD length;
};

class StageLightMovement {
public:
	Vector3 movementDelta;
	bool hasxyRatio;
	float xyRatio;
	bool hasyxRatio;
	float yxRatio;
	DWORD length;
};

class StageLightFlickerType {
public:
	std::string name;
	bool isNull;
	std::vector<StageLightFlicker> flickerEvents;
};

class StageLightRotationType {
public:
	std::string name;
	bool isNull;
	std::vector<StageLightRotation> rotationEvents;
};

class StageLightMovementType {
public:
	std::string name;
	bool isNull;
	std::vector<StageLightMovement> movementEvents;
};

class StageLight {
private:
	Vector3 m_initialLightPosition;
	Vector3 m_initialLightRotation;
	Vector3 m_lightPosition;
	Vector3 m_lightRotation; 
	GTAObject m_lightObject;
	bool m_doTrackPed = false;
	bool m_hasFlicker = false;
	bool m_hasRotation = false;
	bool m_hasMovement = false;
	Ped m_trackPedId = 0;
	int m_trackActorIndex = -1;
	Vector3 m_trackOffset;
	Vector3 m_trackActorLastPos;
	int m_flickerIndex = 0;
	DWORD m_flickerStart=0;
	std::vector<StageLightFlicker> m_flickerEvents;
	int m_rotationIndex = 0;
	DWORD m_rotationStart = 0;
	std::vector<StageLightRotation> m_rotationEvents;
	int m_movementIndex = 0;
	DWORD m_movementStart = 0;
	std::vector<StageLightMovement> m_movementEvents;

public:
	StageLight(Vector3 lightPosition, Vector3 lightRotation, GTAObject lightObject);
	void startTrackActor(Actor actor, int actorIndex);
	void startTrackActor(Actor actor, int actorIndex, Vector3 trackOffset);
	void stopTrackActor();
	int getTrackedActorIndex();
	bool isTrackingActor();
	void removeLightObject();
	void turnOff();
	void turnOn();
	void moveLight(Vector3 lightPosition);
	void moveInitialPosOfLight(Vector3 lightPosition);
	void rotateLight(Vector3 lightRotation);
	void rotateInitialRotationOfLight(Vector3 lightRotation);
	void swapLightObject(GTAObject newLightObject);

	void resetToInitial();

	bool hasFlicker();
	void setHasFlicker(std::vector<StageLightFlicker> flickerEvents);
	std::vector<StageLightFlicker> getFlickerEvents();
	void stopFlicker();

	bool hasRotation();
	void setHasRotation(std::vector<StageLightRotation> rotationEvents);
	std::vector<StageLightRotation> getRotationEvents();
	void stopRotation();

	bool hasMovement();
	void setHasMovement(std::vector<StageLightMovement> movementEvents);
	std::vector<StageLightMovement> getMovementEvents();
	void stopMovement();

	void actionOnTick(DWORD tick, std::vector<Actor> & actors);
	Vector3 getLightPosition();
	Vector3 getLightRotation();
	GTAObject getLightObject();
	Vector3 getTrackedActorOffset();
};


std::vector<GTAObject> getSceneDirectorLightObject();
GTAObject getDefaultSceneDirectorLightObject();
GTAObject getNextSceneDirectorLightObject(GTAObject lightObject);

std::vector<StageLightRotationType> getStageLightRotationTypes();
StageLightRotationType getDefaultStageLightRotationType();
StageLightRotationType getNextStageLightRotationType(StageLightRotationType stageLightEvent);

std::vector<StageLightMovementType> getStageLightMovementTypes();
StageLightMovementType getDefaultStageLightMovementType();
StageLightMovementType getNextStageLightMovementType(StageLightMovementType stageLightEvent);

std::vector<StageLightFlickerType> getStageLightFlickerTypes();
StageLightFlickerType getDefaultStageLightFlickerType();
StageLightFlickerType getNextStageLightFlickerType(StageLightFlickerType stageLightEvent);
void initializeStageLights();
