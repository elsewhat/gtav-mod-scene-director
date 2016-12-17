#include "SyncedAnimation.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

SyncedAnimation::SyncedAnimation()
{
	m_isNull = true;
}

SyncedAnimation::SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, float deltaZLocation)
{
	m_title = title;
	m_actorAnimations = actorAnimations;
	m_deltaZLocation = deltaZLocation;
	m_isNull = false;
}

SyncedAnimation::SyncedAnimation(std::string title, std::vector<Animation>  actorAnimations, std::vector<Animation>  objectAnimations, std::vector<GTAObject>  syncObjects, float deltaZLocation)
{
	m_title = title;
	m_actorAnimations = actorAnimations;
	m_objectAnimations = objectAnimations;
	m_deltaZLocation = deltaZLocation;
	m_syncObjects = syncObjects;
	m_isNull = false;
}

void SyncedAnimation::executeSyncedAnimation(std::vector<Actor*> syncActors, bool useFirstActorLocation, Vector3 directLocation, bool doLoop)
{
	log_to_file("SyncedAnimation->executeSyncedAnimation " + toString());
	DWORD ticksStart = GetTickCount();

	log_to_file("syncActors.size() " + std::to_string(syncActors.size()));
	log_to_file("syncActors[0]->getActorPed() " + std::to_string(syncActors.at(0)->getActorPed()));
	log_to_file("syncActors[0]->getActorPed() " + std::to_string(syncActors.at(0)->getActorPed()));
	log_to_file("syncActors[0]->getActorPed() " + std::to_string(syncActors.at(0)->getActorPed()));

	//must be at least as many actors as animations
	for (int i=0; i < m_actorAnimations.size(); i++) {
		if (syncActors.size() <= i) {
			set_status_text("Must have at least " + std::to_string(m_actorAnimations.size()) + " actors");
			return;
		}
	}

	log_to_file("m_objectAnimation ");
	//must be at least as many objects as object animations
	for (int i = 0; i < m_objectAnimations.size(); i++) {
		if (m_syncObjects.size() <= i || m_syncObjects[i].isNull()) {
			set_status_text("Must have at least " + std::to_string(m_objectAnimations.size()) + " objects");
			return;
		}
	}


	//load actor animations
	for (auto &animation : m_actorAnimations) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
		log_to_file("Loading animation " + animation.toString());
		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
				set_status_text("Failed to load actor animations");
				return;
			}
		}
	}

	//load object animations
	for (auto &animation : m_objectAnimations) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
				set_status_text("Failed to load object animations");
				return;
			}
		}
	}

	log_to_file("About to load objects");
	//load objects
	for (auto &gtaObject : m_syncObjects) {
		if (gtaObject.objHash == -1) {
			gtaObject.objHash = GAMEPLAY::GET_HASH_KEY(gtaObject.objName);
		}

		STREAMING::REQUEST_MODEL(gtaObject.objHash);
		while (!STREAMING::HAS_MODEL_LOADED(gtaObject.objHash))
		{
			WAIT(0);
			if (GetTickCount() - ticksStart > 20000) {
				log_to_file("Failed to load GTAObject name:" + std::string(gtaObject.objName) + " Hash:" + std::to_string(gtaObject.objHash));
				set_status_text("Failed to load objects");
				return;
			}
		}
	}

	//Setup scene for synced animation
	Vector3 sceneLocation = directLocation;

	float startHeading = 0.0;
	if (syncActors.size() > 1) {
		startHeading = ENTITY::GET_ENTITY_HEADING(syncActors.at(0)->getActorPed());
		if (useFirstActorLocation) {
			sceneLocation = ENTITY::GET_ENTITY_COORDS(syncActors.at(0)->getActorPed(), true);
		}
	}
	log_to_file("About to create scene");
	m_sceneId = PED::CREATE_SYNCHRONIZED_SCENE(sceneLocation.x, sceneLocation.y, sceneLocation.z+m_deltaZLocation, 0.0, 0.0, startHeading, 2);
	PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, false);

	log_to_file("About to add animations for actors");
	//Add the animations to the scene
	int actorIndex = 0;
	for (auto &animation : m_actorAnimations) {
		log_to_file("Adding animation to " + std::to_string(syncActors.at(actorIndex)->getActorPed()) + " actor index:" + std::to_string(actorIndex));
		AI::TASK_SYNCHRONIZED_SCENE(syncActors.at(actorIndex)->getActorPed(), m_sceneId, animation.animLibrary, animation.animName, 1000.0, -4.0, 64, 0, 0x447a0000, 0);
		m_pedsInScene.push_back(syncActors.at(actorIndex)->getActorPed());

		m_pedsInSceneStartLocation.push_back(ENTITY::GET_ENTITY_COORDS(syncActors.at(actorIndex)->getActorPed(), true));
		m_pedsInSceneStartHeading.push_back(ENTITY::GET_ENTITY_HEADING(syncActors.at(actorIndex)->getActorPed()));

		actorIndex++;
	}

	log_to_file("About to add animations for objects");
	int objectIndex = 0;
	for (auto &animation : m_objectAnimations) {
		//create object if it doesn't exist
		if (m_syncObjects[objectIndex].objReference == 0) {
			log_to_file("About to create object with name " + std::string(m_syncObjects[objectIndex].objName) + " and hash " + std::to_string(m_syncObjects[objectIndex].objHash));
			int newObjectRef = OBJECT::CREATE_OBJECT(m_syncObjects[objectIndex].objHash, sceneLocation.x, sceneLocation.y, sceneLocation.z, true, true, false);
			m_syncObjects[objectIndex].objReference = newObjectRef;
			m_objectsInScene.push_back(newObjectRef);
		}
		log_to_file("Object animation objReference:" + std::to_string(m_syncObjects[objectIndex].objReference) + " Animation " + animation.animName);
		ENTITY::PLAY_SYNCHRONIZED_ENTITY_ANIM(m_syncObjects[objectIndex].objReference, m_sceneId, animation.animName, animation.animLibrary, 1000.0, -4.0, 0,0x447a0000);
		objectIndex++;
	}

	if (doLoop) {
		PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, doLoop);
	}

	log_to_file("About to execute the synchronized scene");
	//execute the scene
	PED::SET_SYNCHRONIZED_SCENE_PHASE(m_sceneId, 0.0);
}



bool SyncedAnimation::isCompleted()
{
	if (m_sceneId != 0) {
		if (PED::IS_SYNCHRONIZED_SCENE_LOOPED(m_sceneId)) {
			return false;
		}
		float sceneStatus = PED::GET_SYNCHRONIZED_SCENE_PHASE(m_sceneId);
		if (sceneStatus>=1.0) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}

}

void SyncedAnimation::cleanupAfterExecution(bool doDeleteObjects, bool teleportActorsBackToStart)
{
	log_to_file("SyncedAnimation cleanupAfterExecution ");
	int i = 0;
	for (auto &ped : m_pedsInScene) {
		//no idea what 2+3 param is
		ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(ped, 0.0, 1);

		if (teleportActorsBackToStart) {
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ped, m_pedsInSceneStartLocation[i].x, m_pedsInSceneStartLocation[i].y, m_pedsInSceneStartLocation[i].z, 0, 0, 1);
			ENTITY::SET_ENTITY_HEADING(ped, m_pedsInSceneStartHeading[i]);
		}
		i++;
	}
	for (auto &obj : m_objectsInScene) {
		//no idea what 2+3 param is
		ENTITY::STOP_SYNCHRONIZED_ENTITY_ANIM(obj, 0.0, 1);
		if (doDeleteObjects) {
			log_to_file("About to delete object");
			OBJECT::DELETE_OBJECT(&obj);
		}
	}

	for (auto &gtaObject : m_syncObjects) {
		gtaObject.objReference = 0;
	}
	

	log_to_file("SyncedAnimation trying to dispose scene ");
	//dispose scene
	PED::_0xCD9CC7E200A52A6F(m_sceneId);
	m_sceneId = 0;

}

void SyncedAnimation::setLooping(bool doLoop)
{
	if (m_sceneId!=0) {
		log_to_file("Setting looping for scene to " + doLoop);
		PED::SET_SYNCHRONIZED_SCENE_LOOPED(m_sceneId, doLoop);
	}
}


bool SyncedAnimation::matchesFilter(std::string filterStr)
{
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
			if (std::string(m_title).find(token) == std::string::npos) {
				return false;
			}
		}
		else if (currentFitlerOperator == FILTER_NOT) {
			if (std::string(m_title).find(token) != std::string::npos) {
				return false;
			}
		}
		else {//FILTER_OR
			if (std::string(m_title).find(token) != std::string::npos) {
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
}

bool SyncedAnimation::isNull()
{
	return m_isNull;
}

std::string SyncedAnimation::toString()
{
	std::string objString = m_title + " Anims:";
	for (auto &animation : m_actorAnimations) {
		objString += std::to_string(animation.shortcutIndex) + " ";
	}
	if (m_objectAnimations.size() > 0) {
		objString += "ObjectAnims:";
		for (auto &animation : m_objectAnimations) {
			objString += std::to_string(animation.shortcutIndex) + " ";
		}
	}

	objString += "Z-adjustment:" + roundNumber(m_deltaZLocation);


	return objString;
}

float SyncedAnimation::getDeltaZ()
{
	return m_deltaZLocation;
}

void SyncedAnimation::setDeltaZ(float deltaZLocation)
{
	m_deltaZLocation = deltaZLocation;
}

//Defined all SyncedAnimations identified so far

//SyncedAnimation aSyncedAnimation = SyncedAnimation("Test",std::vector<Animation> { getAnimationForShortcutIndex(1801), getAnimationForShortcutIndex(1803) }, 0.0);
std::vector<SyncedAnimation> gtaSyncedAnimations;

void initializeSyncedAnimations() {
	gtaSyncedAnimations = {
		SyncedAnimation("Double doors (two actors)", std::vector<Animation> {getAnimationForShortcutIndex(9683),getAnimationForShortcutIndex(9684),}, std::vector<Animation> {getAnimationForShortcutIndex(9685),getAnimationForShortcutIndex(9686),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_bl_door_l"),getGTAObjectFromObjName("v_ilev_bl_door_r"),}, 0),
		SyncedAnimation("Monkeys freakout", std::vector<Animation> {getAnimationForShortcutIndex(9643),getAnimationForShortcutIndex(9647),getAnimationForShortcutIndex(9651),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Insert chemical", std::vector<Animation> {getAnimationForShortcutIndex(9667),}, std::vector<Animation> {getAnimationForShortcutIndex(9668),getAnimationForShortcutIndex(9669),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_vial_01"),getGTAObjectFromObjName("p_chem_vial_02b_s"),}, 0),
		SyncedAnimation("Take chemical", std::vector<Animation> {getAnimationForShortcutIndex(9632),}, std::vector<Animation> {getAnimationForShortcutIndex(9634),getAnimationForShortcutIndex(9634),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_vial_01"),getGTAObjectFromObjName("p_chem_vial_02b_s"),}, 0),
		SyncedAnimation("Holdup (press button)", std::vector<Animation> {getAnimationForShortcutIndex(9625),getAnimationForShortcutIndex(9627),getAnimationForShortcutIndex(9626),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (scientist walking #2)", std::vector<Animation> {getAnimationForShortcutIndex(9622),getAnimationForShortcutIndex(9621),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (use keycard)", std::vector<Animation> {getAnimationForShortcutIndex(9608),getAnimationForShortcutIndex(9610),getAnimationForShortcutIndex(9609),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Move through labs #2", std::vector<Animation> {getAnimationForShortcutIndex(9606),getAnimationForShortcutIndex(9607),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Driver tells passenger to stop", std::vector<Animation> {getAnimationForShortcutIndex(9605),getAnimationForShortcutIndex(9604),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Driver sad, push passenger away", std::vector<Animation> {getAnimationForShortcutIndex(9603),getAnimationForShortcutIndex(9602),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (scientist walking #1)", std::vector<Animation> {getAnimationForShortcutIndex(9598),getAnimationForShortcutIndex(9601),getAnimationForShortcutIndex(9599),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (scientists working #1)", std::vector<Animation> {getAnimationForShortcutIndex(9587),getAnimationForShortcutIndex(9590),getAnimationForShortcutIndex(9591),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Scientists working #3", std::vector<Animation> {getAnimationForShortcutIndex(9585),getAnimationForShortcutIndex(9586),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Scientists working #2", std::vector<Animation> {getAnimationForShortcutIndex(9581),getAnimationForShortcutIndex(9582),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Scientists working #1", std::vector<Animation> {getAnimationForShortcutIndex(9579),getAnimationForShortcutIndex(9580),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Scientists working base", std::vector<Animation> {getAnimationForShortcutIndex(9583),getAnimationForShortcutIndex(9584),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Move through labs #1", std::vector<Animation> {getAnimationForShortcutIndex(9575),getAnimationForShortcutIndex(9576),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (humane elevator #2)", std::vector<Animation> {getAnimationForShortcutIndex(9570),getAnimationForShortcutIndex(9567),getAnimationForShortcutIndex(9571),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Holdup (humane elevator #1)", std::vector<Animation> {getAnimationForShortcutIndex(9564),getAnimationForShortcutIndex(9569),getAnimationForShortcutIndex(9565),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Driver crying #2", std::vector<Animation> {getAnimationForShortcutIndex(9563),getAnimationForShortcutIndex(9561),}, std::vector<Animation> {getAnimationForShortcutIndex(9559),}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Driver crying #1", std::vector<Animation> {getAnimationForShortcutIndex(9562),getAnimationForShortcutIndex(9560),}, std::vector<Animation> {getAnimationForShortcutIndex(9558),}, std::vector<GTAObject> {getGTAObjectFromObjName("bodhi"),}, 0),
		SyncedAnimation("Take off mask", std::vector<Animation> {getAnimationForShortcutIndex(9508),}, std::vector<Animation> {getAnimationForShortcutIndex(9509),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_trev_ski_mask_s"),}, 0),
		SyncedAnimation("Torture, wrench #3", std::vector<Animation> {getAnimationForShortcutIndex(9476),getAnimationForShortcutIndex(9478),}, std::vector<Animation> {getAnimationForShortcutIndex(9473),getAnimationForShortcutIndex(9480),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_tool_wrench"),}, 0),
		SyncedAnimation("Torture, wrench #2", std::vector<Animation> {getAnimationForShortcutIndex(9467),getAnimationForShortcutIndex(9469),}, std::vector<Animation> {getAnimationForShortcutIndex(9464),getAnimationForShortcutIndex(9471),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_tool_wrench"),}, 0),
		SyncedAnimation("Torture, wrench #1", std::vector<Animation> {getAnimationForShortcutIndex(9458),getAnimationForShortcutIndex(9460),}, std::vector<Animation> {getAnimationForShortcutIndex(9455),getAnimationForShortcutIndex(9462),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("prop_tool_wrench"),}, 0),
		SyncedAnimation("Torture, wrench intro", std::vector<Animation> {getAnimationForShortcutIndex(9485),getAnimationForShortcutIndex(9487),}, std::vector<Animation> {getAnimationForShortcutIndex(9482),getAnimationForShortcutIndex(9488),}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drink whiskey #5 shot", std::vector<Animation> {getAnimationForShortcutIndex(13944),}, std::vector<Animation> {getAnimationForShortcutIndex(13949),getAnimationForShortcutIndex(13951),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("prop_sh_shot_glass"),}, 0),
		SyncedAnimation("Drink whiskey #4 shot", std::vector<Animation> {getAnimationForShortcutIndex(13960),}, std::vector<Animation> {getAnimationForShortcutIndex(13965),getAnimationForShortcutIndex(13967),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("prop_sh_shot_glass"),}, 0),
		SyncedAnimation("Drink whiskey #3 shot", std::vector<Animation> {getAnimationForShortcutIndex(13980),}, std::vector<Animation> {getAnimationForShortcutIndex(13985),getAnimationForShortcutIndex(13987),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("prop_sh_shot_glass"),}, 0),
		SyncedAnimation("Drink whiskey #2 shot", std::vector<Animation> {getAnimationForShortcutIndex(13968),}, std::vector<Animation> {getAnimationForShortcutIndex(13973),getAnimationForShortcutIndex(13975),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("prop_sh_shot_glass"),}, 0),
		SyncedAnimation("Drink whiskey #1 shot", std::vector<Animation> {getAnimationForShortcutIndex(13952),}, std::vector<Animation> {getAnimationForShortcutIndex(13957),getAnimationForShortcutIndex(13959),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("prop_sh_shot_glass"),}, 0),
		SyncedAnimation("Lap dance safehouse", std::vector<Animation> {getAnimationForShortcutIndex(13764),getAnimationForShortcutIndex(13765),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Prison break", std::vector<Animation> {getAnimationForShortcutIndex(13750),getAnimationForShortcutIndex(13748),getAnimationForShortcutIndex(13747),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Kisses", std::vector<Animation> {getAnimationForShortcutIndex(13678),getAnimationForShortcutIndex(13679),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hugs", std::vector<Animation> {getAnimationForShortcutIndex(13675),getAnimationForShortcutIndex(13676),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("High five", std::vector<Animation> {getAnimationForShortcutIndex(13672),getAnimationForShortcutIndex(13673),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Handshake", std::vector<Animation> {getAnimationForShortcutIndex(13669),getAnimationForShortcutIndex(13670),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Shoot out", std::vector<Animation> {getAnimationForShortcutIndex(13633),getAnimationForShortcutIndex(13634),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Walk into vault", std::vector<Animation> {getAnimationForShortcutIndex(13627),}, std::vector<Animation> {getAnimationForShortcutIndex(13628),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_cbankvauldoor01"),}, -0.9),
		SyncedAnimation("Heist ruralbank", std::vector<Animation> {getAnimationForShortcutIndex(13617),getAnimationForShortcutIndex(13618),}, std::vector<Animation> {getAnimationForShortcutIndex(13619),getAnimationForShortcutIndex(13620),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_cbankcountdoor01"),getGTAObjectFromObjName("v_ilev_cbankvauldoor01"),}, -0.9),
		SyncedAnimation("Lamar's introduction to multiplayer male", std::vector<Animation> {getAnimationForShortcutIndex(13530),getAnimationForShortcutIndex(13531),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.5),
		SyncedAnimation("Lamar's introduction to multiplayer female", std::vector<Animation> {getAnimationForShortcutIndex(13528),getAnimationForShortcutIndex(13529),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.5),
		SyncedAnimation("Exit door right", std::vector<Animation> {getAnimationForShortcutIndex(13426),}, std::vector<Animation> {getAnimationForShortcutIndex(13425),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Exit door left", std::vector<Animation> {getAnimationForShortcutIndex(13422),}, std::vector<Animation> {getAnimationForShortcutIndex(13421),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter door right", std::vector<Animation> {getAnimationForShortcutIndex(13418),}, std::vector<Animation> {getAnimationForShortcutIndex(13417),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter door left #2", std::vector<Animation> {getAnimationForShortcutIndex(13414),}, std::vector<Animation> {getAnimationForShortcutIndex(13413),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter door left #1", std::vector<Animation> {getAnimationForShortcutIndex(13411),}, std::vector<Animation> {getAnimationForShortcutIndex(13410),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter house", std::vector<Animation> {getAnimationForShortcutIndex(13339),getAnimationForShortcutIndex(13402),getAnimationForShortcutIndex(13403),}, std::vector<Animation> {getAnimationForShortcutIndex(13401),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, -1),
		SyncedAnimation("Buzz in friend", std::vector<Animation> {getAnimationForShortcutIndex(13392),getAnimationForShortcutIndex(13393),}, std::vector<Animation> {getAnimationForShortcutIndex(13391),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Car bomb install", std::vector<Animation> {getAnimationForShortcutIndex(13235),}, std::vector<Animation> {getAnimationForShortcutIndex(13234),}, std::vector<GTAObject> {getGTAObjectFromObjName("w_ex_pe"),}, 0),
		SyncedAnimation("Arrest on floor front-right", std::vector<Animation> {getAnimationForShortcutIndex(13192),getAnimationForShortcutIndex(13193),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor front-left", std::vector<Animation> {getAnimationForShortcutIndex(13188),getAnimationForShortcutIndex(13189),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor back-right", std::vector<Animation> {getAnimationForShortcutIndex(13184),getAnimationForShortcutIndex(13185),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest on floor back-left", std::vector<Animation> {getAnimationForShortcutIndex(13180),getAnimationForShortcutIndex(13181),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest forward-right", std::vector<Animation> {getAnimationForShortcutIndex(13162),getAnimationForShortcutIndex(13175),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest forward-left", std::vector<Animation> {getAnimationForShortcutIndex(13161),getAnimationForShortcutIndex(13174),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest back-right", std::vector<Animation> {getAnimationForShortcutIndex(13159),getAnimationForShortcutIndex(13173),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arrest back-left", std::vector<Animation> {getAnimationForShortcutIndex(13158),getAnimationForShortcutIndex(13172),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Lap dance", std::vector<Animation> {getAnimationForShortcutIndex(13124),getAnimationForShortcutIndex(13123),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Purchase energydrink", std::vector<Animation> {}, std::vector<Animation> {getAnimationForShortcutIndex(13118),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_ecola_can"),}, 0),
		SyncedAnimation("Purchase cigarette", std::vector<Animation> {getAnimationForShortcutIndex(13117),}, std::vector<Animation> {getAnimationForShortcutIndex(13116),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_fag_packet_01"),}, 0),
		SyncedAnimation("Purchase chocbar", std::vector<Animation> {getAnimationForShortcutIndex(13115),}, std::vector<Animation> {getAnimationForShortcutIndex(13114),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_choc_ego"),}, 0),
		SyncedAnimation("Purchase beerbox", std::vector<Animation> {getAnimationForShortcutIndex(13113),}, std::vector<Animation> {getAnimationForShortcutIndex(13112),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ret_ml_beerjak2"),}, 0),
		SyncedAnimation("Purchase beer", std::vector<Animation> {getAnimationForShortcutIndex(13111),}, std::vector<Animation> {getAnimationForShortcutIndex(13110),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_beer_pissh"),}, 0),
		SyncedAnimation("Store hold up cash", std::vector<Animation> {getAnimationForShortcutIndex(13107),}, std::vector<Animation> {getAnimationForShortcutIndex(13108),getAnimationForShortcutIndex(13109),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_poly_bag_01_s"),getGTAObjectFromObjName("p_till_01_s"),}, 0),
		SyncedAnimation("Jump out of van", std::vector<Animation> {getAnimationForShortcutIndex(12289),getAnimationForShortcutIndex(12290),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.2),
		SyncedAnimation("Dive clear", std::vector<Animation> {getAnimationForShortcutIndex(12281),getAnimationForShortcutIndex(12282),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Shove", std::vector<Animation> {getAnimationForShortcutIndex(12270),getAnimationForShortcutIndex(12271),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Beatup dockworker idle", std::vector<Animation> {getAnimationForShortcutIndex(12261),getAnimationForShortcutIndex(12262),getAnimationForShortcutIndex(12263),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Beatup dockworker exit", std::vector<Animation> {getAnimationForShortcutIndex(12258),getAnimationForShortcutIndex(12259),getAnimationForShortcutIndex(12260),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Beatup dockworker main", std::vector<Animation> {getAnimationForShortcutIndex(12264),getAnimationForShortcutIndex(12265),getAnimationForShortcutIndex(12266),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Beatup dockworker intro", std::vector<Animation> {getAnimationForShortcutIndex(12267),getAnimationForShortcutIndex(12268),getAnimationForShortcutIndex(12269),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("misstrevor3 bike_chat_b_outro_1 outro", std::vector<Animation> {getAnimationForShortcutIndex(12245),getAnimationForShortcutIndex(12246),}, std::vector<Animation> {getAnimationForShortcutIndex(12247),getAnimationForShortcutIndex(12248),}, std::vector<GTAObject> {getGTAObjectFromObjName("daemon"),getGTAObjectFromObjName("daemon"),}, -1),
		SyncedAnimation("misstrevor3 bike_chat_b_loop_1 loop", std::vector<Animation> {getAnimationForShortcutIndex(12240),getAnimationForShortcutIndex(12241),}, std::vector<Animation> {getAnimationForShortcutIndex(12242),getAnimationForShortcutIndex(12243),}, std::vector<GTAObject> {getGTAObjectFromObjName("daemon"),getGTAObjectFromObjName("daemon"),}, -1),
		SyncedAnimation("misstrevor3 bike_chat_a_1 intro", std::vector<Animation> {getAnimationForShortcutIndex(12236),getAnimationForShortcutIndex(12237),}, std::vector<Animation> {getAnimationForShortcutIndex(12238),getAnimationForShortcutIndex(12239),}, std::vector<GTAObject> {getGTAObjectFromObjName("daemon"),getGTAObjectFromObjName("daemon"),}, -1),
		SyncedAnimation("call_biker_biker", std::vector<Animation> {getAnimationForShortcutIndex(12197),getAnimationForShortcutIndex(12198),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("bikers_from_van_peda", std::vector<Animation> {getAnimationForShortcutIndex(12173),getAnimationForShortcutIndex(12174),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.1),
		SyncedAnimation("bikers_from_van_ped_a", std::vector<Animation> {getAnimationForShortcutIndex(12171),getAnimationForShortcutIndex(12172),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.1),
		SyncedAnimation("jump_out_of_van_ds", std::vector<Animation> {getAnimationForShortcutIndex(12169),getAnimationForShortcutIndex(12170),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.1),
		SyncedAnimation("dialogue_b_ron", std::vector<Animation> {getAnimationForShortcutIndex(12157),getAnimationForShortcutIndex(12156),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("threaten_ortega_trev", std::vector<Animation> {getAnimationForShortcutIndex(12107),getAnimationForShortcutIndex(12106),getAnimationForShortcutIndex(12105),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Enter home #2", std::vector<Animation> {getAnimationForShortcutIndex(12066),}, std::vector<Animation> {getAnimationForShortcutIndex(12069),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Enter home #1", std::vector<Animation> {getAnimationForShortcutIndex(12062),}, std::vector<Animation> {getAnimationForShortcutIndex(12064),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fh_door02"),}, 0.1),
		SyncedAnimation("Tattoo #2", std::vector<Animation> {getAnimationForShortcutIndex(14856),getAnimationForShortcutIndex(14857),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Enter shop", std::vector<Animation> {getAnimationForShortcutIndex(12058),getAnimationForShortcutIndex(12059),}, std::vector<Animation> {getAnimationForShortcutIndex(12060),getAnimationForShortcutIndex(12057),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_ta_door"),getGTAObjectFromObjName("v_ilev_ta_door"),}, -1),
		SyncedAnimation("artist_finishes_up_his_tattoo_artist", std::vector<Animation> {getAnimationForShortcutIndex(12055),getAnimationForShortcutIndex(1253),}, std::vector<Animation> {getAnimationForShortcutIndex(12054),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_ta_door"),}, -1),
		SyncedAnimation("shop_ig_4_customer", std::vector<Animation> {getAnimationForShortcutIndex(12042),getAnimationForShortcutIndex(12045),}, std::vector<Animation> {getAnimationForShortcutIndex(12043),getAnimationForShortcutIndex(12044),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_ta_door"),getGTAObjectFromObjName("v_ilev_ta_tatgun"),}, -1),
		SyncedAnimation("mid_mission_inside_helicopter_lester ", std::vector<Animation> {getAnimationForShortcutIndex(12036),getAnimationForShortcutIndex(12037),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Miss switch in car", std::vector<Animation> {getAnimationForShortcutIndex(12031),getAnimationForShortcutIndex(12032),getAnimationForShortcutIndex(12030),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("CPR Cower", std::vector<Animation> {getAnimationForShortcutIndex(10904),getAnimationForShortcutIndex(10905),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("CPR", std::vector<Animation> {getAnimationForShortcutIndex(10902),getAnimationForShortcutIndex(10903),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Arm wrestling winner #2", std::vector<Animation> {getAnimationForShortcutIndex(6210),getAnimationForShortcutIndex(6212),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Arm wrestling winner #1", std::vector<Animation> {getAnimationForShortcutIndex(6206),getAnimationForShortcutIndex(6208),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Motorcycle knockout", std::vector<Animation> {getAnimationForShortcutIndex(5191),getAnimationForShortcutIndex(5192),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Car knockout", std::vector<Animation> {getAnimationForShortcutIndex(5188),getAnimationForShortcutIndex(5189),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drink wheatgrass at safehouse", std::vector<Animation> {getAnimationForShortcutIndex(22897),}, std::vector<Animation> {getAnimationForShortcutIndex(2462),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_w_grass_gls_s"),}, -1),
		SyncedAnimation("Drink wine at safehouse #4", std::vector<Animation> {getAnimationForShortcutIndex(2487),}, std::vector<Animation> {getAnimationForShortcutIndex(2488),getAnimationForShortcutIndex(2490),getAnimationForShortcutIndex(2489),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_02"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wine at safehouse #3", std::vector<Animation> {getAnimationForShortcutIndex(2483),}, std::vector<Animation> {getAnimationForShortcutIndex(2484),getAnimationForShortcutIndex(2486),getAnimationForShortcutIndex(2485),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_02"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wine at safehouse #2", std::vector<Animation> {getAnimationForShortcutIndex(2481),}, std::vector<Animation> {getAnimationForShortcutIndex(22893),getAnimationForShortcutIndex(22894),getAnimationForShortcutIndex(2482),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_01"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink wine at safehouse #1", std::vector<Animation> {getAnimationForShortcutIndex(2477),}, std::vector<Animation> {getAnimationForShortcutIndex(2478),getAnimationForShortcutIndex(2480),getAnimationForShortcutIndex(2479),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_wine_bot_01"),getGTAObjectFromObjName("p_wine_glass_s"),getGTAObjectFromObjName("prop_sh_wine_glass"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #4", std::vector<Animation> {getAnimationForShortcutIndex(2473),}, std::vector<Animation> {getAnimationForShortcutIndex(22895),getAnimationForShortcutIndex(2474),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #3", std::vector<Animation> {getAnimationForShortcutIndex(2470),}, std::vector<Animation> {getAnimationForShortcutIndex(2471),getAnimationForShortcutIndex(2472),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #2", std::vector<Animation> {getAnimationForShortcutIndex(2467),}, std::vector<Animation> {getAnimationForShortcutIndex(2468),getAnimationForShortcutIndex(2469),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Drink whiskey at safehouse #1", std::vector<Animation> {getAnimationForShortcutIndex(22896),}, std::vector<Animation> {getAnimationForShortcutIndex(2465),getAnimationForShortcutIndex(2464),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_cs_whiskey_bottle"),getGTAObjectFromObjName("ex_p_ex_tumbler_01_s"),}, -1),
		SyncedAnimation("Smoke bong beer at safehouse #4", std::vector<Animation> {getAnimationForShortcutIndex(2461),}, std::vector<Animation> {getAnimationForShortcutIndex(22899),getAnimationForShortcutIndex(22901),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("lux_prop_lighter_luxe"),}, -1),
		SyncedAnimation("Smoke bong beer at safehouse #3", std::vector<Animation> {getAnimationForShortcutIndex(2460),}, std::vector<Animation> {getAnimationForShortcutIndex(22898),getAnimationForShortcutIndex(22900),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("ex_prop_exec_lighter_01"),}, -1),
		SyncedAnimation("Smoke bong beer at safehouse #2", std::vector<Animation> {getAnimationForShortcutIndex(2459),}, std::vector<Animation> {getAnimationForShortcutIndex(22903),getAnimationForShortcutIndex(22902),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("v_res_tt_lighter"),}, -1),
		SyncedAnimation("Smoke bong at safehouse #1", std::vector<Animation> {getAnimationForShortcutIndex(2457),}, std::vector<Animation> {getAnimationForShortcutIndex(22904),getAnimationForShortcutIndex(2458),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_bong_01"),getGTAObjectFromObjName("p_cs_lighter_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #4", std::vector<Animation> {getAnimationForShortcutIndex(2454),}, std::vector<Animation> {getAnimationForShortcutIndex(2456),getAnimationForShortcutIndex(2455),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #3", std::vector<Animation> {getAnimationForShortcutIndex(22905),}, std::vector<Animation> {getAnimationForShortcutIndex(2453),getAnimationForShortcutIndex(2454),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #2", std::vector<Animation> {getAnimationForShortcutIndex(2449),}, std::vector<Animation> {getAnimationForShortcutIndex(2451),getAnimationForShortcutIndex(2450),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Drink beer at safehouse #1", std::vector<Animation> {getAnimationForShortcutIndex(2446),}, std::vector<Animation> {getAnimationForShortcutIndex(2448),getAnimationForShortcutIndex(2447),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_sh_beer_pissh_01"),getGTAObjectFromObjName("prop_beer_box_01"),}, -1),
		SyncedAnimation("Rollercoaster hands-up end", std::vector<Animation> {getAnimationForShortcutIndex(2423),getAnimationForShortcutIndex(2424),}, std::vector<Animation> {getAnimationForShortcutIndex(2425),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Rollercoaster hands-up idle", std::vector<Animation> {getAnimationForShortcutIndex(2426),getAnimationForShortcutIndex(30746),}, std::vector<Animation> {getAnimationForShortcutIndex(2427),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Rollercoaster hands-up start", std::vector<Animation> {getAnimationForShortcutIndex(2420),getAnimationForShortcutIndex(2421),}, std::vector<Animation> {getAnimationForShortcutIndex(2422),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Rollercoaster exit", std::vector<Animation> {getAnimationForShortcutIndex(2418),getAnimationForShortcutIndex(2419),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.9),
		SyncedAnimation("Rollercoaster enter", std::vector<Animation> {getAnimationForShortcutIndex(2415),getAnimationForShortcutIndex(2416),}, std::vector<Animation> {getAnimationForShortcutIndex(2417),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_roller_car_01"),}, -0.9),
		SyncedAnimation("Torture, waterboard (outro)", std::vector<Animation> {getAnimationForShortcutIndex(9313),getAnimationForShortcutIndex(9317),}, std::vector<Animation> {getAnimationForShortcutIndex(9306),getAnimationForShortcutIndex(9312),getAnimationForShortcutIndex(9315),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("p_loose_rag_01_s"),}, -0.6),
		SyncedAnimation("Torture, waterboard (loop)", std::vector<Animation> {getAnimationForShortcutIndex(9298),getAnimationForShortcutIndex(9301),}, std::vector<Animation> {getAnimationForShortcutIndex(9294),getAnimationForShortcutIndex(9297),getAnimationForShortcutIndex(9300),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("p_loose_rag_01_s"),}, -0.6),
		SyncedAnimation("Torture, waterboard (kick)", std::vector<Animation> {getAnimationForShortcutIndex(9279),getAnimationForShortcutIndex(9283),}, std::vector<Animation> {getAnimationForShortcutIndex(9275),getAnimationForShortcutIndex(9278),getAnimationForShortcutIndex(9282),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("p_loose_rag_01_s"),}, -0.6),
		SyncedAnimation("Torture, waterboard (intro)", std::vector<Animation> {getAnimationForShortcutIndex(9269),getAnimationForShortcutIndex(9272),}, std::vector<Animation> {getAnimationForShortcutIndex(9265),getAnimationForShortcutIndex(9268),getAnimationForShortcutIndex(9271),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),getGTAObjectFromObjName("w_am_jerrycan"),getGTAObjectFromObjName("p_loose_rag_01_s"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (exit)", std::vector<Animation> {getAnimationForShortcutIndex(9226),getAnimationForShortcutIndex(9229),}, std::vector<Animation> {getAnimationForShortcutIndex(9228),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (loop)", std::vector<Animation> {getAnimationForShortcutIndex(9206),getAnimationForShortcutIndex(9209),}, std::vector<Animation> {getAnimationForShortcutIndex(9208),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (idle)", std::vector<Animation> {getAnimationForShortcutIndex(9190),getAnimationForShortcutIndex(9193),}, std::vector<Animation> {getAnimationForShortcutIndex(9192),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, tooth pull (intro)", std::vector<Animation> {getAnimationForShortcutIndex(9198),getAnimationForShortcutIndex(9201),}, std::vector<Animation> {getAnimationForShortcutIndex(9200),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_pliers_01"),}, -0.6),
		SyncedAnimation("Torture, stab in chest", std::vector<Animation> {getAnimationForShortcutIndex(9174),getAnimationForShortcutIndex(9177),}, std::vector<Animation> {getAnimationForShortcutIndex(9171),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_torture_ch_01"),}, -0.6),
		SyncedAnimation("Group talking", std::vector<Animation> {getAnimationForShortcutIndex(9086),getAnimationForShortcutIndex(9087),getAnimationForShortcutIndex(9088),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Interrogate", std::vector<Animation> {getAnimationForShortcutIndex(8921),getAnimationForShortcutIndex(8923),}, std::vector<Animation> {getAnimationForShortcutIndex(8920),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_off_chair_04"),}, 0),
		SyncedAnimation("Enter car panicked", std::vector<Animation> {getAnimationForShortcutIndex(8817),}, std::vector<Animation> {getAnimationForShortcutIndex(8816),}, std::vector<GTAObject> {getGTAObjectFromObjName("asea"),}, -0.5),
		SyncedAnimation("Ear piercing #3", std::vector<Animation> {getAnimationForShortcutIndex(8751),getAnimationForShortcutIndex(8750),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Ear piercing #2", std::vector<Animation> {getAnimationForShortcutIndex(8744),getAnimationForShortcutIndex(8743),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Ear piercing #1", std::vector<Animation> {getAnimationForShortcutIndex(8741),getAnimationForShortcutIndex(8740),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Ear piercing base", std::vector<Animation> {getAnimationForShortcutIndex(8748),getAnimationForShortcutIndex(8746),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Jimmy drugs michael #2", std::vector<Animation> {getAnimationForShortcutIndex(8735),getAnimationForShortcutIndex(8734),}, std::vector<Animation> {getAnimationForShortcutIndex(8733),}, std::vector<GTAObject> {getGTAObjectFromObjName("tailgater"),}, -0.3),
		SyncedAnimation("Jimmy drugs michael #1", std::vector<Animation> {getAnimationForShortcutIndex(8653),getAnimationForShortcutIndex(8651),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.3),
		SyncedAnimation("Open door #3 (double doors)", std::vector<Animation> {getAnimationForShortcutIndex(8618),}, std::vector<Animation> {getAnimationForShortcutIndex(8617),getAnimationForShortcutIndex(8616),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_mm_doorm_r"),getGTAObjectFromObjName("v_ilev_mm_doorm_l"),}, -1),
		SyncedAnimation("Seated, touching hands", std::vector<Animation> {getAnimationForShortcutIndex(8609),getAnimationForShortcutIndex(8611),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Pull over ledge", std::vector<Animation> {getAnimationForShortcutIndex(8572),getAnimationForShortcutIndex(8573),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1.5),
		SyncedAnimation("Yacht fight 2 (3/3 success)", std::vector<Animation> {getAnimationForShortcutIndex(8547),getAnimationForShortcutIndex(8548),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 2 (3/3 failure)", std::vector<Animation> {getAnimationForShortcutIndex(8539),getAnimationForShortcutIndex(8540),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 2 (2/3)", std::vector<Animation> {getAnimationForShortcutIndex(8544),getAnimationForShortcutIndex(8545),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 2 (1/3)", std::vector<Animation> {getAnimationForShortcutIndex(8541),getAnimationForShortcutIndex(8542),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -2.5),
		SyncedAnimation("Yacht fight 1 (3/3)", std::vector<Animation> {getAnimationForShortcutIndex(8537),getAnimationForShortcutIndex(8538),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -3),
		SyncedAnimation("Yacht fight 1 (2/3)", std::vector<Animation> {getAnimationForShortcutIndex(8535),getAnimationForShortcutIndex(8536),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -3),
		SyncedAnimation("Yacht fight 1 (1/3)", std::vector<Animation> {getAnimationForShortcutIndex(8533),getAnimationForShortcutIndex(8534),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -3),
		SyncedAnimation("Let go of dog collar", std::vector<Animation> {getAnimationForShortcutIndex(8433),getAnimationForShortcutIndex(8432),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Hold dog by collar", std::vector<Animation> {getAnimationForShortcutIndex(8434),getAnimationForShortcutIndex(8435),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Car flips", std::vector<Animation> {getAnimationForShortcutIndex(8424),getAnimationForShortcutIndex(8427),}, std::vector<Animation> {getAnimationForShortcutIndex(8426),}, std::vector<GTAObject> {getGTAObjectFromObjName("dubsta"),}, -1),
		SyncedAnimation("Drunken fist fight", std::vector<Animation> {getAnimationForShortcutIndex(8253),getAnimationForShortcutIndex(8258),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Throw grenade launcher", std::vector<Animation> {getAnimationForShortcutIndex(8238),getAnimationForShortcutIndex(8237),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sex, standing", std::vector<Animation> {getAnimationForShortcutIndex(8134),getAnimationForShortcutIndex(8133),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Blowjob, kneeling", std::vector<Animation> {getAnimationForShortcutIndex(8132),getAnimationForShortcutIndex(8130),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.9),
		SyncedAnimation("Hang and throw over ledge", std::vector<Animation> {getAnimationForShortcutIndex(8127),getAnimationForShortcutIndex(8128),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -9),
		SyncedAnimation("Chad Hug #3", std::vector<Animation> {getAnimationForShortcutIndex(8103),getAnimationForShortcutIndex(8104),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Chad Hug #2", std::vector<Animation> {getAnimationForShortcutIndex(8101),getAnimationForShortcutIndex(8102),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Chad Hug #1", std::vector<Animation> {getAnimationForShortcutIndex(8105),getAnimationForShortcutIndex(8106),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Pull lever", std::vector<Animation> {getAnimationForShortcutIndex(8068),}, std::vector<Animation> {getAnimationForShortcutIndex(8069),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_rail_controller"),}, 0),
		SyncedAnimation("Get in passenger seat", std::vector<Animation> {getAnimationForShortcutIndex(8040),getAnimationForShortcutIndex(8043),}, std::vector<Animation> {getAnimationForShortcutIndex(8042),}, std::vector<GTAObject> {getGTAObjectFromObjName("asea"),}, -0.5),
		SyncedAnimation("Gathered around laptop", std::vector<Animation> {getAnimationForShortcutIndex(8013),getAnimationForShortcutIndex(8014),getAnimationForShortcutIndex(8017),}, std::vector<Animation> {getAnimationForShortcutIndex(8015),getAnimationForShortcutIndex(8016),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_laptop_02_s"),getGTAObjectFromObjName("p_laptop_02_s"),}, -0.4),
		SyncedAnimation("Exit car with rifles", std::vector<Animation> {getAnimationForShortcutIndex(7995),getAnimationForShortcutIndex(7996),}, std::vector<Animation> {getAnimationForShortcutIndex(7994),}, std::vector<GTAObject> {getGTAObjectFromObjName("dubsta"),}, -1.1),
		SyncedAnimation("Take crate from van", std::vector<Animation> {getAnimationForShortcutIndex(7977),}, std::vector<Animation> {getAnimationForShortcutIndex(7939),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_ld_gold_chest"),}, 0),
		SyncedAnimation("Close vault door", std::vector<Animation> {getAnimationForShortcutIndex(7930),}, std::vector<Animation> {getAnimationForShortcutIndex(7931),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_fin_vaultdoor_s"),}, -0.2),
		SyncedAnimation("Tennis coach (practice swing)", std::vector<Animation> {getAnimationForShortcutIndex(7806),getAnimationForShortcutIndex(7810),}, std::vector<Animation> {getAnimationForShortcutIndex(7807),getAnimationForShortcutIndex(7808),getAnimationForShortcutIndex(7809),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_tennis_rack_01"),getGTAObjectFromObjName("prop_tennis_rack_01b"),getGTAObjectFromObjName("p_tennis_bag_01_s"),}, 0.2),
		SyncedAnimation("Gun to driver head (loop)", std::vector<Animation> {getAnimationForShortcutIndex(7801),getAnimationForShortcutIndex(7802),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Gun to driver head (intro)", std::vector<Animation> {getAnimationForShortcutIndex(7794),getAnimationForShortcutIndex(7795),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Open garage door", std::vector<Animation> {getAnimationForShortcutIndex(7755),}, std::vector<Animation> {getAnimationForShortcutIndex(7754),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_gar_door_03_ld"),}, 0),
		SyncedAnimation("Dodge car (push)", std::vector<Animation> {getAnimationForShortcutIndex(7719),getAnimationForShortcutIndex(7720),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dodge car (crowd 3)", std::vector<Animation> {getAnimationForShortcutIndex(7715),getAnimationForShortcutIndex(7716),getAnimationForShortcutIndex(7717),getAnimationForShortcutIndex(7718),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dodge car (crowd 2)", std::vector<Animation> {getAnimationForShortcutIndex(7711),getAnimationForShortcutIndex(7712),getAnimationForShortcutIndex(7713),getAnimationForShortcutIndex(7714),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Dodge car (crowd 1)", std::vector<Animation> {getAnimationForShortcutIndex(7707),getAnimationForShortcutIndex(7708),getAnimationForShortcutIndex(7709),getAnimationForShortcutIndex(7710),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sign in", std::vector<Animation> {getAnimationForShortcutIndex(7620),getAnimationForShortcutIndex(7621),}, std::vector<Animation> {getAnimationForShortcutIndex(7619),}, std::vector<GTAObject> {getGTAObjectFromObjName("p_amb_clipboard_01"),}, -0.58),
		SyncedAnimation("Thrown out", std::vector<Animation> {getAnimationForShortcutIndex(7158),getAnimationForShortcutIndex(7155),getAnimationForShortcutIndex(7156),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Lapdance (Wade)", std::vector<Animation> {getAnimationForShortcutIndex(7161),getAnimationForShortcutIndex(7160),getAnimationForShortcutIndex(7159),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0.2),
		SyncedAnimation("Remove parachute #6 (female)", std::vector<Animation> {getAnimationForShortcutIndex(22478),}, std::vector<Animation> {getAnimationForShortcutIndex(1736),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #5 (female)", std::vector<Animation> {getAnimationForShortcutIndex(22479),}, std::vector<Animation> {getAnimationForShortcutIndex(1734),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #4 (female)", std::vector<Animation> {getAnimationForShortcutIndex(1731),}, std::vector<Animation> {getAnimationForShortcutIndex(1733),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #3 (male)", std::vector<Animation> {getAnimationForShortcutIndex(25111),}, std::vector<Animation> {getAnimationForShortcutIndex(1740),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #2 (male)", std::vector<Animation> {getAnimationForShortcutIndex(25112),}, std::vector<Animation> {getAnimationForShortcutIndex(1739),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Remove parachute #1 (male)", std::vector<Animation> {getAnimationForShortcutIndex(13664),}, std::vector<Animation> {getAnimationForShortcutIndex(22477),}, std::vector<GTAObject> {getGTAObjectFromObjName("hei_p_parachute_s_female"),}, 0),
		SyncedAnimation("Lapdance 2g (touch, dance3)", std::vector<Animation> {getAnimationForShortcutIndex(7103),getAnimationForShortcutIndex(7104),getAnimationForShortcutIndex(7106),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (touch, dance2)", std::vector<Animation> {getAnimationForShortcutIndex(7097),getAnimationForShortcutIndex(7098),getAnimationForShortcutIndex(7100),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (touch, dance1)", std::vector<Animation> {getAnimationForShortcutIndex(7091),getAnimationForShortcutIndex(7092),getAnimationForShortcutIndex(7094),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g (exit, dismiss)", std::vector<Animation> {getAnimationForShortcutIndex(7070),getAnimationForShortcutIndex(7071),getAnimationForShortcutIndex(7073),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g sit-down (pt2)", std::vector<Animation> {getAnimationForShortcutIndex(7084),getAnimationForShortcutIndex(7085),getAnimationForShortcutIndex(7087),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance 2g sit-down (pt1)", std::vector<Animation> {getAnimationForShortcutIndex(7057),getAnimationForShortcutIndex(7058),getAnimationForShortcutIndex(7060),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (idle)", std::vector<Animation> {getAnimationForShortcutIndex(7042),getAnimationForShortcutIndex(7044),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (no touch, dance3)", std::vector<Animation> {getAnimationForShortcutIndex(7039),getAnimationForShortcutIndex(7036),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (no touch, dance2)", std::vector<Animation> {getAnimationForShortcutIndex(7034),getAnimationForShortcutIndex(7031),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (touch, dance3)", std::vector<Animation> {getAnimationForShortcutIndex(7038),getAnimationForShortcutIndex(7036),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (touch, dance2)", std::vector<Animation> {getAnimationForShortcutIndex(7033),getAnimationForShortcutIndex(7031),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (dance1)", std::vector<Animation> {getAnimationForShortcutIndex(7029),getAnimationForShortcutIndex(7027),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance sit-down (sit down pt2)", std::vector<Animation> {getAnimationForShortcutIndex(7025),getAnimationForShortcutIndex(7023),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance sit-down (sit down pt1)", std::vector<Animation> {getAnimationForShortcutIndex(7006),getAnimationForShortcutIndex(7004),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance (start dancing)", std::vector<Animation> {getAnimationForShortcutIndex(7001),getAnimationForShortcutIndex(6999),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance exit (exit, dismiss)", std::vector<Animation> {getAnimationForShortcutIndex(7016),getAnimationForShortcutIndex(7015),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Lapdance exit (exit, touching)", std::vector<Animation> {getAnimationForShortcutIndex(7020),getAnimationForShortcutIndex(7018),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -0.55),
		SyncedAnimation("Enter door", std::vector<Animation> {getAnimationForShortcutIndex(12066),}, std::vector<Animation> {getAnimationForShortcutIndex(12069),}, std::vector<GTAObject> {getGTAObjectFromObjName("v_ilev_fa_frontdoor"),}, 0),
		SyncedAnimation("Gamer with controller and headset", std::vector<Animation> {getAnimationForShortcutIndex(7785),}, std::vector<Animation> {getAnimationForShortcutIndex(7786),getAnimationForShortcutIndex(7787),}, std::vector<GTAObject> {getGTAObjectFromObjName("prop_controller_01"),getGTAObjectFromObjName("prop_headset_01"),}, 0),
		SyncedAnimation("Franklin and Lamar walk to car", std::vector<Animation> {getAnimationForShortcutIndex(7726),getAnimationForShortcutIndex(7727),}, std::vector<Animation> {getAnimationForShortcutIndex(7724),getAnimationForShortcutIndex(7725),}, std::vector<GTAObject> {getGTAObjectFromObjName("ninef2"),getGTAObjectFromObjName("rapidgt2"),}, -1),
		SyncedAnimation("Argueing (Jimmy and Simeon) #2", std::vector<Animation> {getAnimationForShortcutIndex(7705),getAnimationForShortcutIndex(7706),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Argueing (Jimmy and Simeon) #1", std::vector<Animation> {getAnimationForShortcutIndex(7703),getAnimationForShortcutIndex(7704),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Drinking at strip club #4", std::vector<Animation> {getAnimationForShortcutIndex(6974),getAnimationForShortcutIndex(6971),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Drinking at strip club #3", std::vector<Animation> {getAnimationForShortcutIndex(6964),getAnimationForShortcutIndex(6961),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Drinking at strip club #2", std::vector<Animation> {getAnimationForShortcutIndex(6955),getAnimationForShortcutIndex(6952),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Drinking at strip club #1", std::vector<Animation> {getAnimationForShortcutIndex(6940),getAnimationForShortcutIndex(6937),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 1),
		SyncedAnimation("Prostitutes argue", std::vector<Animation> {getAnimationForShortcutIndex(6865),getAnimationForShortcutIndex(6866),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Pimp demands money", std::vector<Animation> {getAnimationForShortcutIndex(6862),getAnimationForShortcutIndex(6864),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Sex low vehicle", std::vector<Animation> {getAnimationForShortcutIndex(6767),getAnimationForShortcutIndex(6766),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drinking with barman #3", std::vector<Animation> {getAnimationForShortcutIndex(6333),getAnimationForShortcutIndex(6336),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drinking with barman #2", std::vector<Animation> {getAnimationForShortcutIndex(6332),getAnimationForShortcutIndex(6335),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Drinking with barman #1", std::vector<Animation> {getAnimationForShortcutIndex(6331),getAnimationForShortcutIndex(6334),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Darts winner #3", std::vector<Animation> {getAnimationForShortcutIndex(6329),getAnimationForShortcutIndex(6330),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Darts winner #2", std::vector<Animation> {getAnimationForShortcutIndex(6326),getAnimationForShortcutIndex(6327),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Darts winner #1", std::vector<Animation> {getAnimationForShortcutIndex(6323),getAnimationForShortcutIndex(6324),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Darts intro #2", std::vector<Animation> {getAnimationForShortcutIndex(6315),getAnimationForShortcutIndex(6317),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0.5),
		SyncedAnimation("Darts intro #1", std::vector<Animation> {getAnimationForShortcutIndex(6306),getAnimationForShortcutIndex(6308),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0.5),
		SyncedAnimation("psycho_rear_takedown_b 3500", std::vector<Animation> {getAnimationForShortcutIndex(6146),getAnimationForShortcutIndex(6148),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_rear_takedown 3366", std::vector<Animation> {getAnimationForShortcutIndex(6145),getAnimationForShortcutIndex(6147),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_front_takedown_c", std::vector<Animation> {getAnimationForShortcutIndex(6113),getAnimationForShortcutIndex(67772),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_front_takedown_b 4000", std::vector<Animation> {getAnimationForShortcutIndex(6144),getAnimationForShortcutIndex(67771),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("psycho_front_takedown 4333", std::vector<Animation> {getAnimationForShortcutIndex(6143),getAnimationForShortcutIndex(67770),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_rear_lefthook 2266", std::vector<Animation> {getAnimationForShortcutIndex(6126),getAnimationForShortcutIndex(6140),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_uppercut 2533", std::vector<Animation> {getAnimationForShortcutIndex(6125),getAnimationForShortcutIndex(6139),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_slap 2399", std::vector<Animation> {getAnimationForShortcutIndex(6124),getAnimationForShortcutIndex(6138),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_low_punch 3066", std::vector<Animation> {getAnimationForShortcutIndex(6123),getAnimationForShortcutIndex(6137),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_headbutt 2266", std::vector<Animation> {getAnimationForShortcutIndex(6122),getAnimationForShortcutIndex(6136),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_elbow 3233", std::vector<Animation> {getAnimationForShortcutIndex(6121),getAnimationForShortcutIndex(6135),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_cross_r 2066", std::vector<Animation> {getAnimationForShortcutIndex(6120),getAnimationForShortcutIndex(6134),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("takedown_front_backslap 2333", std::vector<Animation> {getAnimationForShortcutIndex(6119),getAnimationForShortcutIndex(6133),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_non_lethal_b 2533", std::vector<Animation> {getAnimationForShortcutIndex(6170),getAnimationForShortcutIndex(6176),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_non_lethal_a 3466", std::vector<Animation> {getAnimationForShortcutIndex(6118),getAnimationForShortcutIndex(6132),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_hook_r 2833", std::vector<Animation> {getAnimationForShortcutIndex(6117),getAnimationForShortcutIndex(6131),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("stealth_kill_unarmed_a", std::vector<Animation> {getAnimationForShortcutIndex(6116),getAnimationForShortcutIndex(6130),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee takedown rear lefthook", std::vector<Animation> {getAnimationForShortcutIndex(6062),getAnimationForShortcutIndex(6073),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee takedown front uppercut", std::vector<Animation> {getAnimationForShortcutIndex(6061),getAnimationForShortcutIndex(6072),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee takedown front elbow", std::vector<Animation> {getAnimationForShortcutIndex(6060),getAnimationForShortcutIndex(6071),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee stealth kill unarmed", std::vector<Animation> {getAnimationForShortcutIndex(6059),getAnimationForShortcutIndex(6070),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee failed takedown rear", std::vector<Animation> {getAnimationForShortcutIndex(6058),getAnimationForShortcutIndex(6069),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Melee failed takedown front", std::vector<Animation> {getAnimationForShortcutIndex(6057),getAnimationForShortcutIndex(6068),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife stealth takedown", std::vector<Animation> {getAnimationForShortcutIndex(5827),getAnimationForShortcutIndex(5838),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown front", std::vector<Animation> {getAnimationForShortcutIndex(5826),getAnimationForShortcutIndex(5837),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown front", std::vector<Animation> {getAnimationForShortcutIndex(5825),getAnimationForShortcutIndex(5836),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown failed front", std::vector<Animation> {getAnimationForShortcutIndex(5824),getAnimationForShortcutIndex(5860),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Knife takedown failed rear", std::vector<Animation> {getAnimationForShortcutIndex(5823),getAnimationForShortcutIndex(5859),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hatchet takedown rear #2", std::vector<Animation> {getAnimationForShortcutIndex(5794),getAnimationForShortcutIndex(5797),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hatchet takedown rear #1", std::vector<Animation> {getAnimationForShortcutIndex(5793),getAnimationForShortcutIndex(5796),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Hatchet takedown front", std::vector<Animation> {getAnimationForShortcutIndex(5792),getAnimationForShortcutIndex(5795),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Vehicle knockout passenger (Franklin)", std::vector<Animation> {getAnimationForShortcutIndex(5188),getAnimationForShortcutIndex(5189),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Motorcycle knockout passenger (Franklin)", std::vector<Animation> {getAnimationForShortcutIndex(5191),getAnimationForShortcutIndex(5192),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Retriever takedown front", std::vector<Animation> {getAnimationForShortcutIndex(4426),getAnimationForShortcutIndex(43875),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Retriever takedown back", std::vector<Animation> {getAnimationForShortcutIndex(43873),getAnimationForShortcutIndex(43872),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Cougar takedown front", std::vector<Animation> {getAnimationForShortcutIndex(3814),getAnimationForShortcutIndex(3821),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Cougar takedown back", std::vector<Animation> {getAnimationForShortcutIndex(3812),getAnimationForShortcutIndex(3820),}, std::vector<Animation> {}, std::vector<GTAObject> {}, 0),
		SyncedAnimation("Sarcastic (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1843),getAnimationForShortcutIndex(1845),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sarcastic (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1805),getAnimationForShortcutIndex(1807),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Sarcastic (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1825),getAnimationForShortcutIndex(1827),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("High five (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1839),getAnimationForShortcutIndex(30718),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("High five (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(30708),getAnimationForShortcutIndex(30709),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("High five (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1798),getAnimationForShortcutIndex(30703),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Fistbump (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(30716),getAnimationForShortcutIndex(30717),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Fistbump (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(30706),getAnimationForShortcutIndex(30707),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Fistbump (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(30701),getAnimationForShortcutIndex(30702),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Backslap (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1829),getAnimationForShortcutIndex(30711),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Backslap (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1810),getAnimationForShortcutIndex(30704),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Backslap (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1790),getAnimationForShortcutIndex(30696),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cowering (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(1833),getAnimationForShortcutIndex(30715),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cowering (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1815),getAnimationForShortcutIndex(30705),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Cowering (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(1793),getAnimationForShortcutIndex(30700),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bro hug (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(30712),getAnimationForShortcutIndex(30713),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bro hug (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1812),getAnimationForShortcutIndex(1814),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Bro hug (female - female)", std::vector<Animation> {getAnimationForShortcutIndex(30697),getAnimationForShortcutIndex(30698),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Manly handshake (male - male)", std::vector<Animation> {getAnimationForShortcutIndex(30719),getAnimationForShortcutIndex(30720),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
		SyncedAnimation("Manly handshake (female - male)", std::vector<Animation> {getAnimationForShortcutIndex(1822),getAnimationForShortcutIndex(30710),}, std::vector<Animation> {}, std::vector<GTAObject> {}, -1),
	};
}

std::vector<SyncedAnimation> getAllSyncedAnimations() {
	return gtaSyncedAnimations;
}
