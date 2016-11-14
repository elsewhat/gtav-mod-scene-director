#pragma once

#include "BirdsEyeMode.h"
#include "keyboard.h"
#include "utils.h"
#include <vector>



BirdsEyeMode::BirdsEyeMode()
{
	shouldExitMode = false;
	shouldDrawMenu = true;
	shouldDrawRecordingMarkers = true;
	cameraSpeedFactor = 0.1;
}
/**
* Main loop which is called for each tick from script.cpp
* Returns false if BirdsEyeMode is finished
*/
bool BirdsEyeMode::actionOnTick(DWORD tick, std::vector<Actor> & actors)
{
	disableControls();

	/* ACTIONS WHICH MAY NEED TO WAIT A FEW TICKS */
	if (nextWaitTicks == 0 || GetTickCount() - mainTickLast >= nextWaitTicks) {
		nextWaitTicks = 0;
		shouldExitMode = false;
		if (shouldDrawMenu) {
			if (menu_up_key_pressed()) {
				menu_active_index++;
				nextWaitTicks = 200;
			}
			else if (menu_down_key_pressed()) {
				menu_active_index--;
				nextWaitTicks = 200;
			}
			else if (menu_select_key_pressed()) {
				actionMenuSelected();
			}
		}

		mainTickLast = GetTickCount();

	}
	if (shouldDrawMenu) {
		drawMenu();
		drawInstructions();
	}


	checkInputMovement();
	checkInputRotation();

	//actions to be used during active scene
	draw_spot_lights();

	//check if the player is dead/arrested, in order to swap back to original in order to avoid crash
	check_player_model();

	//check if any recordings should be played
	for (auto & actor : actors) {
		if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
			Actor::update_tick_recording_replay(actor);
		}
		if (shouldDrawRecordingMarkers) {
			actor.drawMarkersForRecording();
		}
	}

	return shouldExitMode;
}

void BirdsEyeMode::onEnterMode(SCENE_MODE aSceneMode)
{
	sceneMode = aSceneMode;
	scaleForm = GRAPHICS::REQUEST_SCALEFORM_MOVIE("instructional_buttons");

	CAM::DO_SCREEN_FADE_OUT(1000);
	WAIT(1000);

	//Find the location of our camera based on the current actor
	Ped actorPed = PLAYER::PLAYER_PED_ID();
	Vector3 startLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);
	float startHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
	Vector3 camOffset;
	camOffset.x = (float)sin((startHeading *PI / 180.0f))*10.0f;
	camOffset.y = (float)cos((startHeading *PI / 180.0f))*10.0f;
	camOffset.z = 12.4;

	if (startLocation.x < 0) {
		camOffset.x = -camOffset.x;
	}
	if (startLocation.y < 0) {
		camOffset.y = -camOffset.y;
	}

	log_to_file("actor location (" + std::to_string(startLocation.x) + ", " + std::to_string(startLocation.y) + ", " + std::to_string(startLocation.z) + ")");
	log_to_file("Camera offset (" + std::to_string(camOffset.x) + ", " + std::to_string(camOffset.y) + ", " + std::to_string(camOffset.z) + ")");

	Vector3 camLocation = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(actorPed, camOffset.x, camOffset.y, camOffset.z);
	log_to_file("Camera location (" + std::to_string(camLocation.x) + ", " + std::to_string(camLocation.y) + ", " + std::to_string(camLocation.z) + ")");
	cameraHandle = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", camLocation.x, camLocation.y, camLocation.z, 0.0, 0.0, 0.0, 40.0, 1, 2);
	//CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
	CAM::POINT_CAM_AT_ENTITY(cameraHandle, actorPed, 0.0f, 0.0f, 0.0f, true);
	CAM::RENDER_SCRIPT_CAMS(true, 0, 3000, 1, 0);
	WAIT(100);
	CAM::STOP_CAM_POINTING(cameraHandle);
	CAM::DO_SCREEN_FADE_IN(1000);
}

void BirdsEyeMode::onExitMode()
{
	CAM::DO_SCREEN_FADE_OUT(1000);
	WAIT(1000);
	//reset cam
	CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);
	WAIT(100);
	CAM::DO_SCREEN_FADE_IN(1000);
}


void BirdsEyeMode::drawMenu() {
	if (menu_active_index > menu_max_index) {
		menu_active_index = 0;
	}
	if (menu_active_index < 0) {
		menu_active_index = menu_max_index;
	}

	int drawIndex = 0;
	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
	

	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	DRAW_TEXT("Exit mode", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_EXIT_BIRDS_EYE_MODE;
	}

	drawIndex++;

	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//Scene mode
	if (sceneMode == SCENE_MODE_ACTIVE) {
		DRAW_TEXT("Scene mode: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}
	else {
		DRAW_TEXT("Scene mode: Setup", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}

	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_SCENE_MODE;
	}
	drawIndex++;

	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	if (shouldDrawRecordingMarkers) {
		DRAW_TEXT("Rec. markers: Show", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}
	else {
		DRAW_TEXT("Rec. markers: Hide", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}

	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_SHOW_REC_MARKERS;
	}

	drawIndex++;


	if (menu_active_index == -1) {
		menu_active_index = 0;
	}

	menu_max_index = drawIndex - 1;
	if (menu_active_index > menu_max_index) {
		menu_active_index = menu_max_index;
	}
}

void BirdsEyeMode::actionMenuSelected() {
	
	if (menu_active_action == MENU_ITEM_SCENE_MODE) {
		nextWaitTicks = 200;
		if (sceneMode == SCENE_MODE_ACTIVE) {
			sceneMode = SCENE_MODE_SETUP;
		}
		else {
			sceneMode = SCENE_MODE_ACTIVE;
		}
		action_toggle_scene_mode();
	}
	else if (menu_active_action == MENU_ITEM_EXIT_BIRDS_EYE_MODE) {
		nextWaitTicks = 200;
		shouldExitMode = true;
	}
	else if (menu_active_action == MENU_ITEM_SHOW_REC_MARKERS) {
		nextWaitTicks = 200;
		shouldDrawRecordingMarkers = !shouldDrawRecordingMarkers;
	}
}


void BirdsEyeMode::drawInstructions() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CLEAR_ALL");

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CREATE_CONTAINER");

		char* altControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 19, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_D");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_A");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_S");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_W");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Move camera");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(1);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Shift: Faster");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(2);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Left/Right mouse btn: Up/Down");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "DRAW_INSTRUCTIONAL_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255,1);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}

void BirdsEyeMode::checkInputRotation()
{
	UI::_SHOW_CURSOR_THIS_FRAME();
	//Obtaining cursor X/Y data to control camera
	float cursorX = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 239);
	float cursorY = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 240);

	//Attempt at handling edge case
	//Does not work
	if (cursorX >= 0.99 || cursorX <= 0.01) {
		CONTROLS::_SET_CONTROL_NORMAL(2, 239, 0.5);
		cursorX = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 239);
	}
	if (cursorY >= 0.99 || cursorY <= 0.01) {
		CONTROLS::_SET_CONTROL_NORMAL(2, 240, 0.5);
		cursorY = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 240);
	}

	//Left/Right camera pan limit
	cursorX *= -720;
	// Up/Down camera tilt limit
	cursorY *= -720;

	if (lastCursorX != cursorX || lastCursorY != cursorY) {
		//Rotate Camera based on Cursor X and Y position
		CAM::SET_CAM_ROT(cameraHandle, cursorY,0.0, cursorX,2);
		
		lastCursorX = cursorX;
		lastCursorY = cursorY;
	}
}


void BirdsEyeMode::checkInputMovement()
{

	Vector3 camDelta = {};

	if (is_key_pressed_for_exit_mode()) {
		shouldExitMode = true;
	}
	else {
		bool isMovement = false;
		if (is_key_pressed_for_forward()) {
			camDelta.x = 1.0;
			isMovement = true;
		}
		if (is_key_pressed_for_backward()) {
			camDelta.x = -1.0;
			isMovement = true;
		}
		if (is_key_pressed_for_left()) {
			camDelta.y = -1.0;
			isMovement = true;
		}
		if (is_key_pressed_for_right()) {
			camDelta.y = 1.0;
			isMovement = true;
		}
		if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(2, 329)) {//LMouseBtn
			camDelta.z = 0.3;
			isMovement = true;
		}
		if (CONTROLS::IS_DISABLED_CONTROL_PRESSED(2, 330) ){//RMouseBtn
			camDelta.z = -0.3;
			isMovement = true;
		}
		if (isMovement) {
			if (is_key_pressed_for_run()) {
				camDelta.x *= 3; 
				camDelta.y *= 3;
				camDelta.z *= 3;
			}

			Vector3 camPos = CAM::GET_CAM_COORD(cameraHandle);
			Vector3 camRot = {};
			camRot= CAM::GET_CAM_ROT(cameraHandle, 2);
			//camera rotation is not as expected. .x value is rotation in the z-plane (view up/down) and third paramter is the rotation in the x,y plane.

			Vector3 direction = {};
			direction = MathUtils::rotationToDirection(camRot);
			//log_to_file("Vector direction (" + std::to_string(direction.x) + ", " + std::to_string(direction.y) + ", " + std::to_string(direction.z) + ")");

			//forward motion
			if (camDelta.x != 0.0) {
				camPos.x += direction.x * camDelta.x * cameraSpeedFactor;
				camPos.y += direction.y * camDelta.x * cameraSpeedFactor;
				camPos.z += direction.z * camDelta.x * cameraSpeedFactor;
			}

			//sideways motion
			if (camDelta.y != 0.0 ) {
				//straight up
				Vector3 b = { };
				b.z = 1.0;

				Vector3 sideWays = {};
				sideWays = MathUtils::crossProduct(direction, b);
				//log_to_file("Vector sideways  (" + std::to_string(sideWays.x) + ", " + std::to_string(sideWays.y) + ", " + std::to_string(sideWays.z) + ")");
				
				camPos.x += sideWays.x * camDelta.y * cameraSpeedFactor;
				camPos.y += sideWays.y * camDelta.y * cameraSpeedFactor;
			}

			//up/down
			if (camDelta.z != 0.0) {
				camPos.z += camDelta.z * cameraSpeedFactor;
			}

			CAM::SET_CAM_COORD(cameraHandle, camPos.x, camPos.y, camPos.z);
		}

	}

}

void BirdsEyeMode::disableControls() {
	std::vector<int> disabledControls = {
		0,2,3,4,5,6,16,17,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,44,45,156,243,257,261,262,263,264,267,268,269,270,271,272,273
	};

	for (auto & controlCode : disabledControls) {
		CONTROLS::DISABLE_CONTROL_ACTION(0, controlCode, 1);
	}
	//INPUT_NEXT_CAMERA = 0,
	//INPUT_LOOK_UD = 2,
	//INPUT_LOOK_UP_ONLY = 3,
	//INPUT_LOOK_DOWN_ONLY = 4,
	//INPUT_LOOK_LEFT_ONLY = 5,
	//INPUT_LOOK_RIGHT_ONLY = 6,
	//INPUT_SELECT_NEXT_WEAPON = 16,
	//INPUT_SELECT_PREV_WEAPON = 17,
	//INPUT_CHARACTER_WHEEL = 19,
	//INPUT_MULTIPLAYER_INFO = 20,
	//INPUT_SPRINT = 21,
	//INPUT_JUMP = 22,
	//INPUT_ENTER = 23,
	//INPUT_ATTACK = 24,
	//INPUT_AIM = 25,
	//INPUT_LOOK_BEHIND = 26,
	//INPUT_PHONE = 27,
	//INPUT_SPECIAL_ABILITY = 28,
	//INPUT_SPECIAL_ABILITY_SECONDARY = 29,
	//INPUT_MOVE_LR = 30,
	//INPUT_MOVE_UD = 31,
	//INPUT_MOVE_UP_ONLY = 32,
	//INPUT_MOVE_DOWN_ONLY = 33,
	//INPUT_MOVE_LEFT_ONLY = 34,
	//INPUT_MOVE_RIGHT_ONLY = 35,
	//INPUT_DUCK = 36,
	//INPUT_SELECT_WEAPON = 37,
	//INPUT_COVER = 44,
	//INPUT_RELOAD = 45,
	//INPUT_MAP = 156,
	//INPUT_ENTER_CHEAT_CODE = 243,
	//INPUT_ATTACK2 = 257,
	//INPUT_PREV_WEAPON = 261,
	//INPUT_NEXT_WEAPON = 262,
	//INPUT_MELEE_ATTACK1 = 263,
	//INPUT_MELEE_ATTACK2 = 264,
	//INPUT_MOVE_LEFT = 266,
	//INPUT_MOVE_RIGHT = 267,
	//INPUT_MOVE_UP = 268,
	//INPUT_MOVE_DOWN = 269,
	//INPUT_LOOK_LEFT = 270,
	//INPUT_LOOK_RIGHT = 271,
	//INPUT_LOOK_UP = 272,
	//INPUT_LOOK_DOWN = 273,

}

bool BirdsEyeMode::is_key_pressed_for_exit_mode() {
	//ALT+ R
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x52)) {
		return true;
	}
	else {
		return false;
	}
}

bool BirdsEyeMode::is_key_pressed_for_forward() {
	//W
	if (IsKeyDown(0x57)) {
		return true;
	}
	else {
		return false;
	}
}
bool BirdsEyeMode::is_key_pressed_for_backward() {
	//W
	if (IsKeyDown(0x53)) {
		return true;
	}
	else {
		return false;
	}
}
bool BirdsEyeMode::is_key_pressed_for_left() {
	//A
	if (IsKeyDown(0x41)) {
		return true;
	}
	else {
		return false;
	}
}
bool BirdsEyeMode::is_key_pressed_for_right() {
	//D
	if (IsKeyDown(0x44)) {
		return true;
	}
	else {
		return false;
	}
}

bool BirdsEyeMode::is_key_pressed_for_run() {
	//D
	if (IsKeyDown(VK_SHIFT)) {
		return true;
	}
	else {
		return false;
	}
}


