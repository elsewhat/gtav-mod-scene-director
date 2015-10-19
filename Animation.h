#pragma once
#include <vector>

struct Animation
{
	int shortcutIndex;
	std::string strShortcutIndex;
	char* animLibrary;
	char* animName;
	int duration;
};

bool initAnimations(std::string fileName);
std::vector<Animation> getAllAnimations();
Animation getAnimationForShortcutIndex(int index);
Animation getAnimationForShortcutIndex(std::string strIndex);