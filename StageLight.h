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
	int m_trackActorIndex = 0;
	Vector3 m_trackOffset;

	void moveLight(Vector3 lightPosition, Vector3 lightRotation);
	void swapLightObject(GTAObject newLightObject);
public:
	StageLight(Vector3 lightPosition, Vector3 lightRotation,float lightHeading, GTAObject lightObject);
	void startTrackActor(Actor actor, int actorIndex);
	void stopTrackActor();
	bool isTrackingActor();
	void removeLightObject();

	bool actionOnTick(DWORD tick, std::vector<Actor> & actors);
};


std::vector<GTAObject> getSceneDirectorLightObject();

GTAObject getDefaultSceneDirectorLightObject();

GTAObject getNextSceneDirectorLightObject(GTAObject lightObject);
