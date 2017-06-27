#pragma once
#include <vector>
#include "GTAObject.h"


class StageLight {
private:
	Vector3 m_lightPosition;
	Vector3 m_lightRotation; 
	GTAObject m_lightObject;
	bool m_doTrackPed = false;
	Ped m_trackPedId = 0;
	int m_trackActorIndex = -1;
	Vector3 m_trackOffset;
	Vector3 m_trackActorLastPos;


public:
	StageLight(Vector3 lightPosition, Vector3 lightRotation, GTAObject lightObject);
	void startTrackActor(Actor actor, int actorIndex);
	void startTrackActor(Actor actor, int actorIndex, Vector3 trackOffset);
	void stopTrackActor();
	int getTrackedActorIndex();
	bool isTrackingActor();
	void removeLightObject();
	void moveLight(Vector3 lightPosition, Vector3 lightRotation);
	void swapLightObject(GTAObject newLightObject);

	void actionOnTick(DWORD tick, std::vector<Actor> & actors);
	Vector3 getLightPosition();
	Vector3 getLightRotation();
	GTAObject getLightObject();
	Vector3 getTrackedActorOffset();
};


std::vector<GTAObject> getSceneDirectorLightObject();

GTAObject getDefaultSceneDirectorLightObject();

GTAObject getNextSceneDirectorLightObject(GTAObject lightObject);
