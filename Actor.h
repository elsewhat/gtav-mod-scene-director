#pragma once
#include <vector>
#include "..\..\inc\types.h"
#include "clipset_movement.h"
#include "lighting.h"
#include "relationship.h"
#include "driving_mode.h"
#include "script.h"
//forward declaration
class ActorRecordingPlayback;
class ActorRecordingItem;

#include "ActorRecording.h"


#include <functional>
#include <memory>
class ActorRecordingPlayback {
protected:
	bool m_hasFirstItemPlayback = false;;
	bool m_playbackCompleted = false;
	bool m_hasTeleportedToStartLocation = false;
	int m_recordingItemIndex;
	int m_maxRecordingItemIndex;
	int m_attemptsCheckCompletion;
	DWORD m_ticksStartCurrentItem;
	DWORD m_ticksLastCheckOfCurrentItem;
	DWORD m_ticksPlaybackStarted;
	DWORD m_ticksTeleportStartLocation;
public:
	ActorRecordingPlayback();
	ActorRecordingPlayback(DWORD tickStart, int nrOfRecordedItems);

	DWORD getTicksPlaybackStarted();

	void setRecordingItemIndex(int index);
	void nextRecordingItemIndex(DWORD ticksNow);
	bool isCurrentRecordedItemLast();
	int getRecordedItemIndex();
	int getNumberOfRecordedItems();

	int getAttemptsCheckedCompletion();
	void incrementAttempstCheckedCompletion();


	void setPlaybackCompleted();
	bool hasPlaybackCompleted();

	void setHasTeleportedToStartLocation(DWORD ticksNow);
	bool hasTeleportedToStartLocation();
	DWORD getTicksTeleportedToStartLocation();

	void setTicksLastCheckOfCurrentItem(DWORD ticks);
	DWORD getTicksLastCheckOfCurrentItem();

	DWORD getTicksStartCurrentItem();

	void setHasFirstItemPlayback(bool hasPlaybacked);
	bool getHasFirstItemPlayback();

	std::string toString();
};


class Actor
{
private:
	Ped m_ped;
	int m_blipId;
	std::string m_name;
	
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
	bool m_hasRecordingWithGunFire = false;

	DWORD m_recordingDelay;


public:
	Actor();
	Actor(Ped ped);

	Ped getActorPed();
	std::string getName();
	void setName(std::string name);

	static Actor nullActor();

	static void update_tick_recording_replay(Actor & actor);

	bool isNullActor();
	bool isActorThisPed(Ped ped);

	void setActiveInScene(SCENE_MODE mode);
	SCENE_MODE isActiveInScene();

	void setBlipId(int blipId);
	int getBlipId();

	void changeActorPed(Ped newPed);

	bool isActorInVehicle(Vehicle veh);

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

	void setHasRecordingWithGunFire(bool hasRecordingWithGunFire);
	bool hasRecordingWithGunFire();

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

	void drawMarkersForRecording(bool showDisabled);

	float distanceToNearestLocation(Vector3 startingPoint);
	std::shared_ptr<ActorRecordingItem> getNearestRecording(Vector3 startingPoint);
};