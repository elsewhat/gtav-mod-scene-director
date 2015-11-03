#pragma once
#include <vector>
#include <string>
#include <windows.h>

struct Animation
{
	int shortcutIndex;
	std::string strShortcutIndex;
	char* animLibrary;
	char* animName;
	int duration;

	std::string toString() {
		return strShortcutIndex + " " + std::string(animLibrary) + " " + std::string(animName) + " " + std::to_string(duration);
	}
};


struct AnimationSequence
{
	DWORD keyShortcut;
	DWORD keyShortcut2;

	std::vector<Animation> animationsInSequence;

};


struct AnimationFlag
{
	std::string name;
	int id;

	bool isFacialAnimation() {
		if (id == -1) {
			return true;
		}
		else {
			return false;
		}
	}
};

enum ANIMATION_FLAGS
{
	ANIMATION_LOOP_FLAG1 = 1,
	ANIMATION_FLAG2 = 2,
	ANIMATION_FLAG3 = 4,
	ANIMATION_FLAG4 = 8,
	ANIMATION_UPPER_BODY_FLAG5 = 16,
	ANIMATION_ALLOW_MOVEMENT_FLAG6 = 32,
	ANIMATION_FLAG7 = 64,
	ANIMATION_ALLOW_WEAPONS_FLAG8 = 128,
	ANIMATION_CRAZY_FLAG9 = 256,
	ANIMATION_FLAG10 = 512,
	ANIMATION_FLAG11 = 1024,
	ANIMATION_FLAG12 = 2048,
	ANIMATION_FLAG13 = 4096,
	ANIMATION_CANCEL_ON_MOVEMENT_FLAG14 = 8162,
	ANIMATION_FLAG15 = 16384,
	ANIMATION_FLAG16 = 32768,
	ANIMATION_FLAG17 = 65536,
	ANIMATION_FLAG18 = 131072,
	ANIMATION_FLAG19 = 262144,
};

bool initAnimations(std::string fileName);
std::vector<Animation> getAllAnimations();
Animation getAnimationForShortcutIndex(int index);
Animation getAnimationForShortcutIndex(std::string strIndex);

std::vector<AnimationFlag> getAnimationFlags();
AnimationFlag getDefaultAnimationFlag();
AnimationFlag getNextAnimationFlag(AnimationFlag animationFlag);