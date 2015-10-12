#pragma once
#include <vector>
#include "..\..\inc\types.h"
#include "clipset_movement.h"
#include "lighting.h"
#include "relationship.h"
#include "driving_mode.h"
#include "script.h"
#include "ActorRecording.h"

//forward declaration
class ActorRecordingPlayback;
class ActorRecordingItem;

class Actor
{
private:
	Ped m_ped;
	int m_blipId;
	
	SCENE_MODE m_sceneStatus;
	
	bool m_hasWaypoint;
	Vector3 m_waypoint;

	bool m_hasStartLocation;
	Vector3 m_startLocation;
	float m_startLocationHeading;
	
	bool m_hasSpotlight;
	SPOT_LIGHT_TYPE m_spotlightType;
	SpotLightColor m_spotlightColor;
	
	bool m_hasWalkingStyle;
	ClipSet m_walkingStyle;

	RelationshipGroup m_relationshipGroup;
	
	bool m_vehicleNoDamage;
	
	float m_walkingSpeed;
	
	DrivingMode m_drivingMode;

	bool m_hasRecording;
	std::vector<ActorRecordingItem> m_actorRecordingItems;
	bool m_replayRecordingInProgress;
	ActorRecordingPlayback m_actorRecordingPlayback;


public:
	Actor();
	Actor(Ped ped);

	Ped getActorPed();

	static Actor nullActor();

	bool isNullActor();
	bool isActorThisPed(Ped ped);

	void setActiveInScene(SCENE_MODE mode);
	SCENE_MODE isActiveInScene();

	void setBlipId(int blipId);
	int getBlipId();

	void setHasWaypoint(bool hasWaypoint); 
	bool hasWaypoint();

	void setWaypoint(Vector3 location);
	Vector3 getWaypoint();

	void setHasStartLocation(bool hasStartLocation);
	bool hasStartLocation();

	void setStartLocation(Vector3 location);
	Vector3 getStartLocation();

	void setStartLocationHeading(float heading);
	float getStartLocationHeading();
	
	void setHasSpotLight(bool hasSpot);
	bool hasSpotLight();

	void setSpotLightType(SPOT_LIGHT_TYPE type);
	SPOT_LIGHT_TYPE getSpotLightType();

	void setSpotLightColor(SpotLightColor color);
	SpotLightColor getSpotLightColor();

	void setHasWalkingStyle(bool hasWalkingStyle);
	bool hasWalkingStyle();

	void setWalkingStyle(ClipSet walkingStyle);
	ClipSet getWalkingStyle();

	void setRelationshipGroup(RelationshipGroup group);
	RelationshipGroup getRelationshipGroup();

	void setVehicleNoDamage(bool noDamage);
	bool hasVehicleNoDamage();

	void setWalkingSpeed(float speed);
	float getWalkingSpeed();

	void setDrivingMode(DrivingMode drivingMode);
	DrivingMode getDrivingMode();

	void setHasRecording(bool hasRecording);
	bool hasRecording();

	void setRecording(std::vector<ActorRecordingItem> actorRecordingItems);
	std::vector<ActorRecordingItem> getRecording();
	ActorRecordingItem getRecordingAt(int index);

	void startReplayRecording(DWORD tickStart);
	void stopReplayRecording();
	void resumeReplayRecording();

	bool isCurrentlyPlayingRecording();

	ActorRecordingPlayback & getRecordingPlayback();
};