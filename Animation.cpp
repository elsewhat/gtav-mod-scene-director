#include "Animation.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>


std::vector<AnimationFlag> gtaAnimationFlags = {
	{ "Flag1",ANIMATION_LOOP_FLAG1 },
	{ "Flag2",ANIMATION_FLAG2 },
	{ "Flag1+2",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG2 },
	{ "Flag3",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG3 },
	{ "Flag4",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG4 },
	{ "Flag5",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 },
	{ "Flag6",ANIMATION_LOOP_FLAG1 | ANIMATION_ALLOW_MOVEMENT_FLAG6 },
	{ "Flag7",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG7 },
	{ "Flag8",ANIMATION_LOOP_FLAG1 | ANIMATION_ALLOW_WEAPONS_FLAG8 },
	{ "Flag9",ANIMATION_LOOP_FLAG1 | ANIMATION_CRAZY_FLAG9 },
	{ "Flag10",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG10 },
	{ "Flag11",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG11 },
	{ "Flag12",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG12 },
	{ "Flag13",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG13 },
	{ "Flag14",ANIMATION_LOOP_FLAG1 | ANIMATION_CANCEL_ON_MOVEMENT_FLAG14 },
	{ "Flag15",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG15 },
	{ "Flag16",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG16},
	{ "Flag17",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG17 },
	{ "Flag18",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG18 },
	{ "Flag19",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG19 },

};

/*
std::vector<AnimationFlag> gtaAnimationFlags = {
	{ "Full body 33",33 },
	{ "Upper body 49",49 },
};*/

std::vector<Animation> gtaAnimations ={
 { 0,"00000","","",0 } 
};



std::vector<Animation> getAllAnimations() {
	return gtaAnimations;
}

bool initAnimations(std::string fileName) {
	gtaAnimations.reserve(21883);


	std::ifstream animationsFile;
	animationsFile.open(fileName);

	if (!animationsFile){
		return false;
	}
	int index = 1;
	std::string strShortcutIndex;
	std::string animLibrary;
	std::string animName;
	int duration;

	while (animationsFile >> strShortcutIndex >> animLibrary >> animName >> duration)
	{
		gtaAnimations.push_back({ index,strShortcutIndex,strdup(animLibrary.c_str()),strdup(animName.c_str()),duration });
		index++;
	}


	return true;
}

Animation getAnimationForShortcutIndex(int index) {
	if (index > 0 && index < gtaAnimations.size()) {
		return gtaAnimations[index];
	}
	else {
		//0 is the null animation
		return gtaAnimations[0];
	}
}

Animation getAnimationForShortcutIndex(std::string strIndex) {
	strIndex.erase(0, strIndex.find_first_not_of('0'));
	int index = atoi(strIndex.c_str());
	return getAnimationForShortcutIndex(index);
}


std::vector<AnimationFlag> getAnimationFlags() {
	return gtaAnimationFlags;
}

AnimationFlag getDefaultAnimationFlag() {
	return gtaAnimationFlags[0];
}

AnimationFlag getNextAnimationFlag(AnimationFlag animationFlag) {
	//see http://stackoverflow.com/questions/14225932/search-for-a-struct-item-in-a-vector-by-member-data
	int foundIndex = std::find_if(gtaAnimationFlags.begin(), gtaAnimationFlags.end(), [=](AnimationFlag const& flag) {
		return (flag.id == animationFlag.id);
	}) - gtaAnimationFlags.begin();
	if (foundIndex + 1 >= gtaAnimationFlags.size()) {//color not found or in last element
		return gtaAnimationFlags[0];
	}
	else {
		return gtaAnimationFlags[foundIndex + 1];
	}
}