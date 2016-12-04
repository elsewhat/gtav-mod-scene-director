#include "Animation.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>


bool Animation::matchesFilter(std::string filterStr){
	std::vector<std::string> filterTokens = StringUtils::split(filterStr, ' ');

	FILTER_OPERATORS currentFitlerOperator = FILTER_OR;
	bool matchesFilter = true;
	int nrMatches = 0;
	for (auto token : filterTokens) {
		if (!token.empty() && (token.compare("AND") == 0 || token.compare("and") == 0)) {
			currentFitlerOperator = FILTER_AND;
			continue;
		}
		else if (!token.empty() && (token.compare("OR") == 0 || token.compare("or") == 0)) {
			currentFitlerOperator = FILTER_OR;
			continue;
		}
		else if (!token.empty() && (token.compare("NOT") == 0 || token.compare("not") == 0)) {
			currentFitlerOperator = FILTER_NOT;
			continue;
		}

		if (currentFitlerOperator == FILTER_AND) {
			if (std::string(animName).find(token) == std::string::npos && std::string(animLibrary).find(token) == std::string::npos) {
				return false;
			}
		}
		else if (currentFitlerOperator == FILTER_NOT) {
			if (std::string(animName).find(token) != std::string::npos || std::string(animLibrary).find(token) != std::string::npos) {
				return false;
			}
		}
		else {//FILTER_OR
			if (std::string(animName).find(token) != std::string::npos || std::string(animLibrary).find(token) != std::string::npos) {
				nrMatches++;
			}
		}
	}

	if (nrMatches > 0) {
		return true;
	}
	else {
		return false;
	}

	/*if (std::string(animName).find(filterStr) != std::string::npos || std::string(animLibrary).find(filterStr) != std::string::npos) {
	return true;
	}
	else {
	return false;
	}*/
}

std::vector<AnimationFlag> gtaAnimationFlags = {
	{ "Normal",ANIMATION_LOOP_FLAG1 },
	{ "Controllable*",ANIMATION_LOOP_FLAG1 | ANIMATION_ALLOW_MOVEMENT_FLAG6 | ANIMATION_UPPER_BODY_FLAG5 },
	{ "Facial",ANIMATION_LOOP_FLAG1 | ANIMATION_ALLOW_MOVEMENT_FLAG6 },
	{ "Stop on movement",ANIMATION_LOOP_FLAG1 | ANIMATION_STOP_ON_MOVEMENT_FLAG8 },
	{ "Crazy #1",ANIMATION_LOOP_FLAG1 | ANIMATION_CRAZY_FLAG9 },
	{ "Crazy #2",ANIMATION_LOOP_FLAG1 | ANIMATION_FLAG14 },

	/*
	{ "Test 1",ANIMATION_LOOP_FLAG1  | ANIMATION_UPPER_BODY_FLAG5 },
	{ "Test 7",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG7},
	{ "Test 10",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG10 },
	{ "Test 11",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG11 },
	{ "Test 12",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG12 },
	{ "Test 13",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG13 },
	{ "Test 14", ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG14 },
	{ "Test 15",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG15 },
	{ "Test 16",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG16 },
	{ "Test 17",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG17 }, 
	{ "Test 18",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG18 },
	{ "Test 19",ANIMATION_LOOP_FLAG1 | ANIMATION_UPPER_BODY_FLAG5 | ANIMATION_FLAG19 },*/
};


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