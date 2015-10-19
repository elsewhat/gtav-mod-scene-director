#include "Animation.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

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



