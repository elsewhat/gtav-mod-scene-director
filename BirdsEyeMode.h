#pragma once

#include "..\..\inc\natives.h"
#include "..\..\inc\types.h"
#include <vector>
#include "script.h"
#include "actor.h"
#include "StageLight.h"

class BirdsEyeMode {
private:
	Any cameraHandle;
	int scaleForm;
	float cameraSpeedFactor = 0.1;
	bool shouldExitMode = false;
	bool shouldDrawRecordingMarkers = true;
	bool invertedControls = true;
	int menu_active_index = 0;
	int menu_max_index = 0;
	MENU_ITEM menu_active_action = MENU_ITEM_SCENE_MODE;
	int submenu_active_index = -1;
	int submenu_max_index = 0;
	bool submenu_is_active = false;
	MENU_ITEM submenu_active_action = SUBMENU_ITEM_EDIT_LOCATION;

	SCENE_MODE sceneMode = SCENE_MODE_SETUP;
	DWORD nextWaitTicks = 0;
	DWORD mainTickLast = 0;
	Vector3 camLastPos;
	Vector3 camNewPos;
	bool lastChangedRecordingLocation = false;
	bool showDisabled = true;

	std::shared_ptr<ActorRecordingItem> nearestRecording=nullptr;
	std::shared_ptr<Actor> nearestActor = nullptr;
	std::shared_ptr<ActorRecordingItem> highlightedRecording = nullptr;
	std::shared_ptr<Actor> highlightedActor = nullptr;
	std::shared_ptr<ActorRecordingItem> selectedRecording = nullptr;
	int selectedRecordingIndex = 0;
	std::shared_ptr<Actor> selectedActor = nullptr;
	void drawInstructions();
	std::shared_ptr<ActorRecordingItem> getNearestRecording(std::vector<Actor> & actors);
	void drawRecordingMarkers(std::vector<Actor> & actors);
	void drawMenu();
	void drawSubMenuEdit();
	void disableControls();
	void actionMenuSelected();
	void actionSubMenuEditSelected();
	void actionToggleEditLocation();
	float actionInputFloat();
	DWORD actionInputDword();
	void actionInputWalkSpeed();
	void actionInputVehicleSpeed();
	void actionInputMinDistance();
	void actionInputDeltaCheck();
	void actionInputNrAttemptsBeforeSkipping();
	void actionInputAnimationRecording();
	bool checkInputAction();
	bool checkInputMovement();
	bool checkInputRotation();
	std::shared_ptr<ActorRecordingItem> getActiveRecordingItem();
	std::shared_ptr<Actor> getActiveActor();
	bool is_key_pressed_for_select_item();
	bool is_key_pressed_for_exit_mode();
	bool is_key_pressed_for_forward();
	bool is_key_pressed_for_backward();
	bool is_key_pressed_for_left();
	bool is_key_pressed_for_right();
	bool is_key_pressed_for_run();
	bool is_key_pressed_for_crouch();
	bool is_key_pressed_for_inverted_cam();
	bool is_key_pressed_for_add_stage_light();
	bool is_key_pressed_for_clear_all_stage_lights();
public:
	BirdsEyeMode();
	void onEnterMode(SCENE_MODE aSceneMode);
	void onExitMode();
	bool actionOnTick(DWORD tick, std::vector<Actor> & actors, std::vector<StageLight> & sceneStageLights);
};
