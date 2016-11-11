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
	bool shouldExitMode = false;;
	void drawInstructions();
	void checkInputMovement();
	void checkInputRotation();
	bool is_key_pressed_for_exit_mode();
	bool is_key_pressed_for_forward();
	bool is_key_pressed_for_backward();
	bool is_key_pressed_for_left();
	bool is_key_pressed_for_right();
public:
	BirdsEyeMode();
	void onEnterMode();
	void onExitMode();
	bool actionOnTick(DWORD tick, std::vector<Actor> actors);
};
