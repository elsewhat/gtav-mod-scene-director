#include "clipset_movement.h"
#include <vector>

std::vector<ClipSet> gtaDrunk = {
	{"MOVE_M@DRUNK@SLIGHTLYDRUNK", "Tipsy" },
	{"MOVE_M@DRUNK@MODERATEDRUNK", "Drunk" },
	{"MOVE_M@DRUNK@MODERATEDRUNK_HEAD_UP","Drunk 2"  },
	{ "MOVE_M@DRUNK@VERYDRUNK","Very drunk" },
};

std::vector<ClipSet> getAllDrunkClipSet() {
	return gtaDrunk;
}