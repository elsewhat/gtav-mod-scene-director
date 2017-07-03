#pragma once
#include <vector>
#include "GTAObject.h"



class StageLightFlicker {
public:
	bool isOn;
	DWORD length;
};

class StageLight {
private:
	Vector3 m_lightPosition;
	Vector3 m_lightRotation; 
	GTAObject m_lightObject;
	bool m_doTrackPed = false;
	bool m_hasFlicker = false;
	Ped m_trackPedId = 0;
	int m_trackActorIndex = -1;
	Vector3 m_trackOffset;
	Vector3 m_trackActorLastPos;
	int m_flickerIndex = 0;
	DWORD m_flickerStart=0;
	std::vector<StageLightFlicker> m_flickerEvents;

public:
	StageLight(Vector3 lightPosition, Vector3 lightRotation, GTAObject lightObject);
	void startTrackActor(Actor actor, int actorIndex);
	void startTrackActor(Actor actor, int actorIndex, Vector3 trackOffset);
	void stopTrackActor();
	int getTrackedActorIndex();
	bool isTrackingActor();
	bool hasFlicker();
	void removeLightObject();
	void turnOff();
	void turnOn();
	void moveLight(Vector3 lightPosition, Vector3 lightRotation);
	void swapLightObject(GTAObject newLightObject);

	void setHasFlicker(std::vector<StageLightFlicker> flickerEvents);
	void stopFlicker();

	void actionOnTick(DWORD tick, std::vector<Actor> & actors);
	Vector3 getLightPosition();
	Vector3 getLightRotation();
	GTAObject getLightObject();
	Vector3 getTrackedActorOffset();
};


std::vector<GTAObject> getSceneDirectorLightObject();

GTAObject getDefaultSceneDirectorLightObject();

GTAObject getNextSceneDirectorLightObject(GTAObject lightObject);
