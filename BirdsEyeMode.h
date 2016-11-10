#pragma once

#include "..\..\inc\natives.h"
#include "..\..\inc\types.h"
#include "script.h"

class BirdsEyeMode {
private:
	Any cameraHandle;
	int scaleForm;
	bool shouldExitMode = false;;
	void drawInstructions();
	void checkInputMovement();
	void checkInputRotation();
	bool is_key_pressed_for_exit_mode();
	bool is_key_pressed_for_forward();
	bool is_key_pressed_for_backward();
	bool is_key_pressed_for_left();
	bool is_key_pressed_for_right();
	Vector3 rotationToDirection(Vector3 rotation);
	Vector3 crossProduct(Vector3 a, Vector3 b);
public:
	BirdsEyeMode();
	void onEnterMode();
	void onExitMode();
	bool actionOnTick(DWORD tick);
};
