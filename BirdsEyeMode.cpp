#pragma once

#include "BirdsEyeMode.h"
#include "keyboard.h"
#include "utils.h"



BirdsEyeMode::BirdsEyeMode()
{
	shouldExitMode = false;
	cameraSpeedFactor = 0.1;
}
/**
* Main loop which is called for each tick from script.cpp
* Returns false if BirdsEyeMode is finished
*/
bool BirdsEyeMode::actionOnTick(DWORD tick, std::vector<Actor> actors)
{
	shouldExitMode = false;
	drawInstructions();
	checkInputMovement();
	checkInputRotation();


	//actions to be used during active scene
	draw_spot_lights();

	//check if the player is dead/arrested, in order to swap back to original in order to avoid crash
	//check_player_model();

	//check if any recordings should be played
	for (auto & actor : actors) {
		if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
			Actor::update_tick_recording_replay(actor);
		}
	}

	return shouldExitMode;
}

void BirdsEyeMode::onEnterMode()
{
	scaleForm = GRAPHICS::_REQUEST_SCALEFORM_MOVIE2("instructional_buttons");

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




void BirdsEyeMode::drawInstructions() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		GRAPHICS::_CALL_SCALEFORM_MOVIE_FUNCTION_VOID(scaleForm, "CLEAR_ALL");

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_CALL_SCALEFORM_MOVIE_FUNCTION_VOID(scaleForm, "CREATE_CONTAINER");

		char* altControlKey = CONTROLS::_0x0499D7B09FC9B407(2, 19, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_D");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_A");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_S");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_W");
		
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Move camera");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "DRAW_INSTRUCTIONAL_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_RGBA(scaleForm, 255, 255, 255, 255);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}

void BirdsEyeMode::checkInputRotation()
{
	UI::_SHOW_CURSOR_THIS_FRAME();
	//Obtaining cursor X/Y data to control camera
	//GTA.Control.CursorX == 239  GTA.Control.CursorY ==240
	float mouseX = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 239);
	float mouseY = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 240);

	if (mouseX >= 0.99 || mouseX <= 0.01) {
		CONTROLS::_0xE8A25867FBA3B05E(0, 239, 0.5);
		mouseX = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 239);
	}
	if (mouseY >= 0.99 || mouseY <= 0.01) {
		CONTROLS::_0xE8A25867FBA3B05E(0, 240, 0.5);
		mouseX = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 240);
	}

	// Left/Right camera pan limit (Based on screen's width)
	mouseX *= -720;

	// Up/Down camera tilt limit (This prevents the camera from tilting up into the plane)
	mouseY *= -720;

	//Is Mouse Being Used?
	//GTA.Control.LookUpDown== 2  GTA.Control.LookLeftRight ==1
	bool mouseUD = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 2);
	bool mouseLR = CONTROLS::GET_DISABLED_CONTROL_NORMAL(0, 1);

	if (mouseUD || mouseLR) {
		//Rotate Camera based on Cursor X and Y position
		CAM::SET_CAM_ROT(cameraHandle, mouseY,0.0, mouseX,2);
		
		Vector3 camRot = { 0.0, 0.0, 0.0 };
		camRot = CAM::GET_CAM_ROT(cameraHandle, 2);
		Vector3 direction = { 0.0, 0.0, 0.0 };
		direction = MathUtils::rotationToDirection(camRot);
	}
}


void BirdsEyeMode::checkInputMovement()
{
	//disable MoveLeftRight and MoveUpDown https://github.com/crosire/scripthookvdotnet/blob/dev_v3/source/scripting/Controls.cs
	//CONTROLS::DISABLE_CONTROL_ACTION(0, 30, 1);
	//CONTROLS::DISABLE_CONTROL_ACTION(0, 31, 1);

	//disable all controls
	CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);

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


