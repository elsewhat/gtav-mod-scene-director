#pragma once
#include <vector>
#include "..\..\inc\types.h"
#include "clipset_movement.h"
#include "lighting.h"
#include "relationship.h"
#include "driving_mode.h"
#include "script.h"
#include "ActorRecording.h"

#include <functional>
#include <memory>

//forward declaration
class ActorRecordingPlayback;
class ActorRecordingItem;

class Actor
{
private:
	Ped m_ped;
	int m_blipId;
	
	SCENE_MODE m_sceneStatus;
	
	bool m_hasWaypoint=false;
	Vector3 m_waypoint;

	bool m_hasStartLocation=false;
	Vector3 m_startLocation;
	float m_startLocationHeading;
	Vehicle m_startLocationVehicle;
	Vector3 m_startLocationVehicleLocation;
	float m_startLocationVehicleHeading;
	int m_startLocationVehicleSeat = -1; 
	
	bool m_hasSpotlight = false;
	SPOT_LIGHT_TYPE m_spotlightType;
	SpotLightColor m_spotlightColor;
	
	bool m_hasWalkingStyle = false;
	ClipSet m_walkingStyle;

	RelationshipGroup m_relationshipGroup;
	
	bool m_vehicleNoDamage = false;
	
	float m_walkingSpeed;
	
	DrivingMode m_drivingMode;

	bool m_hasRecording = false;
	std::vector<std::shared_ptr<ActorRecordingItem>> m_actorRecordingItems;
	bool m_replayRecordingInProgress = false;
	ActorRecordingPlayback m_actorRecordingPlayback;

	DWORD m_recordingDelay;

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

	void changeActorPed(Ped newPed);

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

	void setRecordingDelay(DWORD ticks);
	DWORD getRecordingDelay();

	void setStartLocationVehicle(Vehicle vehicle, Vector3 startLocationVehicleLocation,float startLocationVehicleHeading);
	Vehicle getStartLocationVehicle();
	Vector3 getStartLocationVehicleLocation();
	bool hasStartLocationVehicle();
	float getStartLocationVehicleHeading();

	void setStartLocationVehicleSeat(int seat);
	int getStartLocationVehicleSeat();

	void setRecording(std::vector<std::shared_ptr<ActorRecordingItem>> actorRecordingItems);
	std::vector<std::shared_ptr<ActorRecordingItem>> getRecording();
	std::shared_ptr<ActorRecordingItem> getRecordingAt(int index);

	void startReplayRecording(DWORD tickStart);
	void stopReplayRecording();
	void resumeReplayRecording();

	bool isCurrentlyPlayingRecording();

	void removeRecording();

	ActorRecordingPlayback& getRecordingPlayback();
	void setRecordingPlayback(ActorRecordingPlayback recordingPlayback);
};