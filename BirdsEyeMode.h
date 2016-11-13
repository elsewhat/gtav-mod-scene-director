#pragma once

#include "..\..\inc\natives.h"
#include "..\..\inc\types.h"
#include <vector>
#include "script.h"
#include "actor.h"

class BirdsEyeMode {
private:
	Any cameraHandle;
	int scaleForm;
	float cameraSpeedFactor = 0.1;
	bool shouldExitMode = false;
	bool shouldDrawMenu = true;
	bool shouldDrawRecordingMarkers = true;
	int menu_active_index = 0;
	int menu_max_index = 0;
	MENU_ITEM menu_active_action = MENU_ITEM_SCENE_MODE;
	SCENE_MODE sceneMode = SCENE_MODE_SETUP;
	DWORD nextWaitTicks = 0;
	DWORD mainTickLast = 0;
	float lastCursorX = 0.0;
	float lastCursorY = 0.0;
	void drawInstructions();
	void drawMenu();
	void disableControls();
	void actionMenuSelected();
	void checkInputMovement();
	void checkInputRotation();
	bool is_key_pressed_for_exit_mode();
	bool is_key_pressed_for_forward();
	bool is_key_pressed_for_backward();
	bool is_key_pressed_for_left();
	bool is_key_pressed_for_right();
	bool is_key_pressed_for_run();
public:
	BirdsEyeMode();
	void onEnterMode(SCENE_MODE aSceneMode);
	void onExitMode();
	bool actionOnTick(DWORD tick, std::vector<Actor> & actors);
};
