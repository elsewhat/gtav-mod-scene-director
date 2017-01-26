#pragma once

#include "script.h"
#include "keyboard.h"
#include "utils.h"
#include "scenario.h"
#include "weather.h"
#include "clipset_movement.h"
#include "lighting.h"
#include "relationship.h"
#include "driving_mode.h"
#include "ActorRecording.h"
#include "Actor.h"
#include "Animation.h"
#include "ActorProp.h"
#include "BirdsEyeMode.h"
#include "GTAObject.h"
#include "SyncedAnimation.h"
#include "tinyxml2.h"
#include "StageLight.h"

#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include <memory>
#include <bitset> 
#include <map>



//bool isBitSet = IntBits(value).test(position);

//Key attributes
//These are overwrittein in init_read_keys_from_ini()
DWORD key_hud = VK_F10;
DWORD key_menu_up = VK_NUMPAD8;
DWORD key_menu_down = VK_NUMPAD2;
DWORD key_menu_left = VK_NUMPAD4;
DWORD key_menu_right = VK_NUMPAD6;
DWORD key_menu_select = VK_NUMPAD5;
DWORD key_menu_delete = VK_DELETE;
DWORD key_menu_back = VK_NUMPAD0;
char key_hud_str[256];





enum SCENE_DIRECTOR_MODES {
	MODE_STANDARD=1,
	MODE_BIRDS_EYE_VIEW = 2,
};

SCENE_DIRECTOR_MODES activeMode = MODE_STANDARD;

bool should_display_hud = false;

std::vector<Actor>  actors(20);
bool actor0IsClone = false;
Actor previousActor = Actor::nullActor();

AnimationFlag animationFlag = getDefaultAnimationFlag();
Animation animationPrevious{ 0,"00000","","",0 };

std::vector<AnimationTrigger>  animationSequences;
int animationSequencesIndex = 0;

std::vector<SyncedAnimation*>  syncedAnimationsInRecordings;

bool hasAnimationFilter = false;
std::string animationFilterStr;
bool doAnimationLoop = false;


bool is_autopilot_engaged_for_player = false;
bool is_chase_player_engaged = false;
bool is_escort_player_engaged = false;

bool is_firing_squad_engaged = false;

DWORD actorHashGroup = 0x5F0783F1;
Hash* actorHashGroupP = &actorHashGroup;

//scaleform instructional_buttons reference
int scaleForm; 

//if menu_active_index ==-1 , then menu_active_ped is used to show selected index
int menu_active_index = 0;
MENU_ITEM menu_active_action = MENU_ITEM_SCENE_MODE;
int menu_max_index = 0;
Ped menu_active_ped = 0;

bool submenu_is_displayed = false;
bool submenu_is_active = false;
int submenu_active_index = -1;
int submenu_max_index = 0;
MENU_ITEM submenu_active_action = MENU_ITEM_SCENE_MODE;

SCENE_MODE sceneMode = SCENE_MODE_SETUP;

std::vector<Scenario> gtaScenarios;

//used in passenger waypoint
int lastWaypointID = -1;

//used for world -> blackout
bool is_world_blackout = false;
bool is_timlapse_active = false;
int timelapse_delta_seconds = 0;
int timelapse_delta_minutes = 2;
DWORD timelapseLastTick = 0;
DWORD timelapseDeltaTicks = 100;

std::vector<Weather> gtaWeatherTypes;
int index_weather = -1;
bool is_wind_active = false;

bool do_record_reload = false;


std::vector<RelationshipGroup> modRelationshipGroups;

std::vector<std::pair<std::string,int>> modSyncedAnimCategories;
std::string currentSyncedAnimCategory;
std::vector<SyncedAnimation> currentSyncedAnimations;

std::vector<ClipSet> gtaWalkingStyles;
int index_walking_style = -1;

ActorProp currentActorProp = getDefaultActorProp();

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

//Used for determining wait time after actions
DWORD mainTickLast=0;
DWORD nextWaitTicks = 0;

int forceSlotIndexOverWrite = -1;

std::string saveFileName = "SceneDirector_save.xml";

//used to automatically switch over current actor over to new Ped in check_player_model
Ped lastPlayerPed = 0;

BirdsEyeMode birdsEyeController;

std::vector<StageLight> sceneStageLights;



TaskSequence currentTaskSequence = 100;

void set_status_text(std::string text)
{
	UI::_SET_NOTIFICATION_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)text.c_str());
	UI::_DRAW_NOTIFICATION(1, 1);
}

//log and config file handling borrowed from https://github.com/amoshydra/bearded-batman/blob/e814cf559edbb24b1ef80a326d0608ff67ba17cb/source/Kinky/script.cpp


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

	return buf;
}

void log_to_file(std::string message, bool bAppend) {
	if (1) {
		std::ofstream logfile;
		char* filename = "scene_director.log";
		if (bAppend)
			logfile.open(filename, std::ios_base::app);
		else
			logfile.open(filename);
		//logfile << currentDateTime() << " " << message + "\n";
		logfile << message + "\n";


		logfile.close();
	}
	return;
}


void action_show_info_on_start(){
	set_status_text("Scene director 3.3 by elsewhat");
	set_status_text("Duplicate actors in Rockstar editor? Restart GTA after recording");
	set_status_text("Scene is setup mode");
}

// Config file - test

LPCSTR config_path = ".\\scene_director.ini";
//int test_property = GetPrivateProfileInt("keys", "test_key", 42, config_path);


static LPCSTR weaponNames[] = {
	"WEAPON_KNIFE", "WEAPON_NIGHTSTICK", "WEAPON_HAMMER", "WEAPON_BAT", "WEAPON_GOLFCLUB", "WEAPON_CROWBAR",
	"WEAPON_PISTOL", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL", "WEAPON_PISTOL50", "WEAPON_MICROSMG", "WEAPON_SMG",
	"WEAPON_ASSAULTSMG", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE", "WEAPON_ADVANCEDRIFLE", "WEAPON_MG",
	"WEAPON_COMBATMG", "WEAPON_PUMPSHOTGUN", "WEAPON_SAWNOFFSHOTGUN", "WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN",
	"WEAPON_STUNGUN", "WEAPON_SNIPERRIFLE", "WEAPON_HEAVYSNIPER", "WEAPON_GRENADELAUNCHER", "WEAPON_GRENADELAUNCHER_SMOKE",
	"WEAPON_RPG", "WEAPON_MINIGUN", "WEAPON_GRENADE", "WEAPON_STICKYBOMB", "WEAPON_SMOKEGRENADE", "WEAPON_BZGAS",
	"WEAPON_MOLOTOV", "WEAPON_FIREEXTINGUISHER", "WEAPON_PETROLCAN",
	"WEAPON_SNSPISTOL", "WEAPON_SPECIALCARBINE", "WEAPON_HEAVYPISTOL", "WEAPON_BULLPUPRIFLE", "WEAPON_HOMINGLAUNCHER",
	"WEAPON_PROXMINE", "WEAPON_SNOWBALL", "WEAPON_VINTAGEPISTOL", "WEAPON_DAGGER", "WEAPON_FIREWORK", "WEAPON_MUSKET",
	"WEAPON_MARKSMANRIFLE", "WEAPON_HEAVYSHOTGUN", "WEAPON_GUSENBERG", "WEAPON_HATCHET", "WEAPON_RAILGUN",
	"WEAPON_COMBATPDW", "WEAPON_KNUCKLE", "WEAPON_MARKSMANPISTOL"
};

void give_all_weapons(Player playerPed) {
	for (int i = 0; i < sizeof(weaponNames) / sizeof(weaponNames[0]); i++)
		WEAPON::GIVE_DELAYED_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char *)weaponNames[i]), 1000, 0);
	//set_status_text("all weapons added");
}

void give_basic_weapon(Player playerPed) {
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(playerPed, GAMEPLAY::GET_HASH_KEY((char *)weaponNames[7]), 1000, 0);
}

Actor& get_actor_from_ped(Ped ped) {
	for (auto &actor : actors) {
		if (actor.isActorThisPed(ped)) {
			return actor;
		}
	}
	static Actor nullActor = Actor::nullActor();
	return nullActor;
}

int get_actor_index_from_ped(Ped ped) {
	int i = -1;
	for (auto &actor : actors) {
		if (actor.isActorThisPed(ped)) {
			return i;
		}
		i++;
	}
	return i;
}

std::vector<Actor*> getActorPointers() {
	std::vector<Actor *> actorsPoint;
	for (int i = 0; i < actors.size(); i++) {
		if (!actors[i].isNullActor()) {
			actorsPoint.push_back(&actors[i]);
			log_to_file("ActorPed:" + std::to_string(actors[i].getActorPed()));
		}
		
	}

	for (auto actor : actorsPoint) {
		log_to_file("ActorPed*:" + std::to_string(actor->getActorPed()));
	}

	return actorsPoint;
}

// player model control, switching on normal ped model when needed	
void check_player_model()
{
	// common variables
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (playerPed != lastPlayerPed && lastPlayerPed!=0) {
		//assume that Player has swapped model through trainer if old ped does not exist
		//swap Ped id for current actor if possible
		if (!ENTITY::DOES_ENTITY_EXIST(lastPlayerPed)) {
			Actor & actor = get_actor_from_ped(lastPlayerPed);
			if (actor.isNullActor() == false) {
				actor.changeActorPed(playerPed);
				set_status_text("Update current actor after switching model");
			}
		}

	}
	lastPlayerPed = playerPed;

	if (!ENTITY::DOES_ENTITY_EXIST(playerPed)) return;

	Hash model = ENTITY::GET_ENTITY_MODEL(playerPed);
	if (ENTITY::IS_ENTITY_DEAD(playerPed) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
		if (model != GAMEPLAY::GET_HASH_KEY("player_zero") &&
			model != GAMEPLAY::GET_HASH_KEY("player_one") &&
			model != GAMEPLAY::GET_HASH_KEY("player_two"))
		{
			//set_status_text("turning to normal");
			WAIT(1000);

			model = GAMEPLAY::GET_HASH_KEY("player_zero");
			STREAMING::REQUEST_MODEL(model);
			while (!STREAMING::HAS_MODEL_LOADED(model))
				WAIT(0);
			PLAYER::SET_PLAYER_MODEL(PLAYER::PLAYER_ID(), model);
			PED::SET_PED_DEFAULT_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID());
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(model);

			// wait until player is ressurected
			while (ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()) || PLAYER::IS_PLAYER_BEING_ARRESTED(player, TRUE))
				WAIT(0);

		}
}

void DRAW_TEXT(char* Text, float X, float Y, float S_X, float S_Y, int Font, bool Outline, bool Shadow, bool Center, bool RightJustify, int R, int G, int B, int A)
{
	UI::SET_TEXT_FONT(Font);
	UI::SET_TEXT_SCALE(S_X, S_Y);
	UI::SET_TEXT_COLOUR(R, G, B, A);
	if (Outline)
		UI::SET_TEXT_OUTLINE();
	if (Shadow)
		UI::SET_TEXT_DROP_SHADOW();
	UI::SET_TEXT_CENTRE(Center);
	UI::SET_TEXT_RIGHT_JUSTIFY(RightJustify);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING(Text);
	UI::_DRAW_TEXT(X, Y);
}




void store_current_waypoint_for_actor(Ped ped) {

	if (UI::IS_WAYPOINT_ACTIVE()) {
		log_to_file("store_current_waypoint_for_actor: Looking for ped with id " + std::to_string(ped));
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		//ignore the first index

		Actor & actor = get_actor_from_ped(ped);
		if (actor.isNullActor() == false) {
			log_to_file("store_current_waypoint_for_actor:Found match");
			actor.setHasWaypoint(true);
			actor.setWaypoint(waypointCoord);
		}
		else {
			log_to_file("store_current_waypoint_for_actor: Found no match");
		}
	}
}



int get_next_free_slot() {
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i].isNullActor()) {
			return i+1;
		}
	}
	return -1;
}


bool is_ped_actor_active(Ped ped) {
	Actor & actor = get_actor_from_ped(ped);
	if (actor.isNullActor() == false) {
		return actor.isActiveInScene();
	}
	else {
		//if ped is not in a slot, return the status of the overall scene
		if (sceneMode == SCENE_MODE_SETUP) {
			return false;
		}
		else {
			return true;
		}
	}
}


bool should_display_app_hud() {
	return should_display_hud;
}


bool hud_toggle_key_pressed()
{
	return IsKeyJustUp(key_hud);
}




void ensure_ped_and_vehicle_is_not_deleted(Ped ped) {
	if (!ENTITY::DOES_ENTITY_EXIST(ped)){
		return;
	}

	ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ped,true,true);
	if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(pedVehicle, true, true);
	}
}


void ensure_max_driving_ability(Ped ped) {
	PED::SET_DRIVER_ABILITY(ped, 1.0);
	PED::SET_DRIVER_AGGRESSIVENESS(ped, 1.0);
}

void create_relationship_groups() {
	log_to_file("set_relationships_between_actors");

	//Group A hates Group B, but like Group C
	RelationshipGroup groupA = modRelationshipGroups[0];
	//Group B hates Group A, but like Group C
	RelationshipGroup groupB = modRelationshipGroups[1];
	//Group C likes Group A and Group B
	RelationshipGroup groupC = modRelationshipGroups[2];

	Hash groupAHash = GAMEPLAY::GET_HASH_KEY(groupA.id);
	Hash groupBHash = GAMEPLAY::GET_HASH_KEY(groupB.id);
	Hash groupCHash = GAMEPLAY::GET_HASH_KEY(groupC.id);

	PED::REMOVE_RELATIONSHIP_GROUP(groupAHash);
	PED::REMOVE_RELATIONSHIP_GROUP(groupBHash);
	PED::REMOVE_RELATIONSHIP_GROUP(groupCHash);

	PED::ADD_RELATIONSHIP_GROUP(groupA.id, &(groupAHash));
	PED::ADD_RELATIONSHIP_GROUP(groupB.id, &(groupBHash));
	PED::ADD_RELATIONSHIP_GROUP(groupC.id, &(groupCHash));

	/*Relationship types:
		0 = Companion
		1 = Respect
		2 = Like
		3 = Neutral
		4 = Dislike
		5 = Hate
		*/
	//PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, groupA.actorHash, GAMEPLAY::GET_HASH_KEY("player"));
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, groupAHash, groupAHash);
	//PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GAMEPLAY::GET_HASH_KEY("player"), groupA.actorHash);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupAHash, groupBHash);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, groupAHash, groupCHash);

	//PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, groupB.actorHash, GAMEPLAY::GET_HASH_KEY("player"));
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, groupBHash, groupBHash);
	//PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GAMEPLAY::GET_HASH_KEY("player"), groupB.actorHash);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupBHash, groupAHash);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, groupBHash, groupCHash);

	//PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, groupC.actorHash, GAMEPLAY::GET_HASH_KEY("player"));
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, groupCHash, groupCHash);
	//PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GAMEPLAY::GET_HASH_KEY("player"), groupC.actorHash);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, groupCHash, groupAHash);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, groupCHash, groupBHash);

}

void assign_actor_to_relationship_group(Ped ped, RelationshipGroup relationshipGroup) {
	log_to_file("Adding actor to relationship group . Existing hash is " +std::to_string(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped))+ " new has should be is " + std::to_string(GAMEPLAY::GET_HASH_KEY(relationshipGroup.id)));
	
	if(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped) == relationshipGroup.actorHash) {
		log_to_file("Ped already belongs to actor relationship group");
	}else {
		PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, GAMEPLAY::GET_HASH_KEY(relationshipGroup.id));
		//PED::SET_PED_RELATIONSHIP_GROUP_DEFAULT_HASH(ped, relationshipGroup.actorHash);
	}
	log_to_file("Relationship group after add " + std::to_string(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped)));
}

void draw_instructional_buttons() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		//_instructionalButtonsScaleform.CallFunction("CLEAR_ALL");
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CLEAR_ALL");

		//_instructionalButtonsScaleform.CallFunction("TOGGLE_MOUSE_BUTTONS", 0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		//_instructionalButtonsScaleform.CallFunction("CREATE_CONTAINER");
		//not used in scripts, not sure if required
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CREATE_CONTAINER");

		//_instructionalButtonsScaleform.CallFunction("SET_DATA_SLOT", 0, Function.Call<string>(Hash._0x0499D7B09FC9B407, 2, (int)Control.PhoneSelect, 0), "Select");

		char* altControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 19, 1);
		char* spaceControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 22, 1);
		char* delControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 178, 1);
		char* insControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 121, 1);
		char* endControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 179, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		std::string scaleFormKey = "t_" + std::string(key_hud_str);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING(strdup(scaleFormKey.c_str()));
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Hide HUD menu");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(1);
		GRAPHICS::_0xE83A3E3557A56640(delControlKey);
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Back to start");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(2);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_F");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Enter as passenger");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();


		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(3);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("END Explode nearby veh");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();




		/* Clone moved to menu
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_F10");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Clone player");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();
		*/
		
		/* Possess moved to menu
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(1);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_F9");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Possess ped");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();
		*/

		/* Waypoint to all less focus now
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(2);
		GRAPHICS::_0xE83A3E3557A56640(insControlKey);
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Waypoint to all");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();
		*/

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(4);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_A");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Autopilot");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();


		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(5);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_C");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Firing squad");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();



		/* Hiding scene mode
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(6);
		GRAPHICS::_0xE83A3E3557A56640(spaceControlKey);
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Scene mode");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();
		*/

		//_instructionalButtonsScaleform.CallFunction("DRAW_INSTRUCTIONAL_BUTTONS", -1);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "DRAW_INSTRUCTIONAL_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		//causes the stability bug in NativeUI
		//invoke<Void>(0xCF537FDE4FBD4CE5, scaleForm, 255, 255, 255, 255);
		GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255,1);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}

void draw_instructional_buttons_player_recording() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CLEAR_ALL");

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CREATE_CONTAINER");

		char* altControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 19, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_E");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Stop recording");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(1);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_F");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Enter as passenger");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(2);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_J");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Speak (hold)");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "DRAW_INSTRUCTIONAL_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255,1);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}

void draw_instructional_buttons_animation_preview() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CLEAR_ALL");

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CREATE_CONTAINER");

		char* altControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 19, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_C");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Stop preview");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(1);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_N");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Next");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(2);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_B");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Previous");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(3);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(197);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Rotate cam");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(4);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(196);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Rotate cam");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(5);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_G");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Go to anim");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(6);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_L");
		if (doAnimationLoop) {
			GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Loop: On");
		}
		else {
			GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Loop: Off");
		}

		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(7);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_F");
		std::string filterInstruction = "Filter";
		if (hasAnimationFilter) {
			filterInstruction += ": " + animationFilterStr;
		}

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING(strdup(filterInstruction.c_str()));
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "DRAW_INSTRUCTIONAL_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255,1);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}

void draw_instructional_buttons_animation_sync_preview() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CLEAR_ALL");

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::CALL_SCALEFORM_MOVIE_METHOD(scaleForm, "CREATE_CONTAINER");

		char* altControlKey = CONTROLS::_GET_CONTROL_ACTION_NAME(2, 19, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_C");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Stop preview");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(1);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(197);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Rotate cam");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(3);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(196);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Rotate cam");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(4);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_+");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Up");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(5);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_-");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Down");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(6);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_A");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Add to shortcut");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "DRAW_INSTRUCTIONAL_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(-1);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleForm, 255, 255, 255, 255, 1);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}


void draw_submenu_animation(int drawIndex) {
	int submenu_index = 0;


	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;


	for (int i = 0; i < animationSequences.size();i++) {
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = (MENU_ITEM) (SUBMENU_ITEM_ANIMATION_SEQUENCE_0 + i);
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		std::string textPrefix = "Anim";
		if (animationSequences[i].isSyncedAnimation()) {
			textPrefix = "SynchAnim";
		}
			
		char* animText = strdup((textPrefix + std::to_string(i + 1) + " ALT+NUM" + std::to_string(i + 1)).c_str());
		if ((i + 1) >= 10) {
			animText = strdup((textPrefix + " #"+ std::to_string(i + 1)).c_str());
		}

		DRAW_TEXT(animText, 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;
	}

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ANIMATION_FLAG;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	DRAW_TEXT(strdup(("Type: "+ animationFlag.name).c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ANIMATION_SEQUENCE;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	DRAW_TEXT("Add single animation", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ANIMATION_PREVIEW;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	DRAW_TEXT("Single animation preview", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ANIMATION_SYNC;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	DRAW_TEXT("Test new synch. anim", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ANIMATION_SYNC_PREVIEW;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	DRAW_TEXT("Synchronized animation", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	/*
	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ACTOR_PROP_ADD;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	DRAW_TEXT("Add prop to actor", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_ACTOR_PROP_SELECT;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	DRAW_TEXT(strdup(("Prop: " + currentActorProp.name).c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	*/

	submenu_max_index = submenu_index;

}

void draw_submenu_world(int drawIndex) {
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Actor & playerActor = get_actor_from_ped(playerPed);
	int submenu_index = 0;
	
	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_RECORD_RELOAD;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	char* recordReloadText = "Record reload: No";
	if (do_record_reload) {
		recordReloadText = "Record reload: Yes";
	}
	DRAW_TEXT(recordReloadText, 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;


	//5. If chase is engaged
	if (is_chase_player_engaged) {
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_CHASE;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		DRAW_TEXT("Player chase: Active", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;
	}
	else if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_CHASE;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		DRAW_TEXT("Activate chase", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;
	}


	//6. If escort is engaged
	if (is_escort_player_engaged) {
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_ESCORT;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		DRAW_TEXT("Player escort: Active", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;
	}
	else if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_ESCORT;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		DRAW_TEXT("Activate escort", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;
	}


	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_BLACKOUT;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	char* blackoutText = "Blackout";
	if (is_world_blackout) {
		blackoutText = "Blackout: Active";
	}
	DRAW_TEXT(blackoutText, 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_TIMELAPSE;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	char* timelapseText = "Timelapse";
	if (is_timlapse_active) {
		timelapseText = "Timelapse: Active";
	}
	DRAW_TEXT(timelapseText, 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_WEATHER;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	char* weatherText = "Weather";
	if (index_weather!=-1) {
		Weather weather = gtaWeatherTypes[index_weather];
		std::string weatherTextString = "Weather: " + std::string(weather.id);
		weatherText = strdup(weatherTextString.c_str());
	}
	DRAW_TEXT(weatherText, 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_WIND;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	char* windText = "Wind";
	if (is_wind_active) {
		windText = "Wind: Active";
	}
	DRAW_TEXT(windText, 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);



	submenu_max_index = submenu_index;

}
void draw_submenu_save_load(int drawIndex) {
	int submenu_index = 0;

	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_SAVE_ACTORS;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	DRAW_TEXT("Save actors", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_LOAD_ACTORS;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	DRAW_TEXT("Load actors", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	submenu_max_index = submenu_index;

}


void draw_submenu_player(int drawIndex) {
	int submenu_index = 0;
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());

	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		if (actor.hasRecording()) {
			submenu_active_action = SUBMENU_ITEM_DELETE_RECORDING;
		}
		else {
			submenu_active_action = SUBMENU_ITEM_RECORD_PLAYER;
		}
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	if (actor.hasRecording()) {
		DRAW_TEXT("Remove recording", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	else {
		DRAW_TEXT("Record - Scene setup", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	if (!actor.hasRecording()) {
		drawIndex++;
		submenu_index++;

		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_RECORD_PLAYER_WOTHERS;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		DRAW_TEXT("Record - Scene active", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}


	if (actor.hasRecording() && !actor.isCurrentlyPlayingRecording()) {
		drawIndex++;
		submenu_index++;

		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_TEST_RECORDING;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		DRAW_TEXT("Test recording", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;

		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_RECORDING_DELAY;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		std::string recordingDelayStr = "Rec. delay: " + std::to_string(actor.getRecordingDelay());

		DRAW_TEXT(strdup(recordingDelayStr.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

		drawIndex++;
		submenu_index++;

		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_COPY_REC_TO_OTHERS;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}


		DRAW_TEXT("Copy rec. to other actors", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		
	}

	if (actor.isCurrentlyPlayingRecording()) {
		drawIndex++;
		submenu_index++;

		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_IS_PLAYING_RECORDING;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		int currentIndex = actor.getRecordingPlayback().getRecordedItemIndex()+1;
		int maxIndex = actor.getRecordingPlayback().getNumberOfRecordedItems();
		std::string playbackStatus = "Playback: " + std::to_string(currentIndex) + " / " + std::to_string(maxIndex);
		DRAW_TEXT(strdup(playbackStatus.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}


	drawIndex++;
	submenu_index++;

	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_REMOVE_FROM_SLOT;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	DRAW_TEXT("Remove from slot", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_HEALTH;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	std::string healthText = "Health: " +std::to_string(ENTITY::GET_ENTITY_MAX_HEALTH(actor.getActorPed()));
	DRAW_TEXT(strdup(healthText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);



	if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
		drawIndex++;
		submenu_index++;
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_VEHICLE_COSMETIC;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		std::string vehCosmeticText = "Vehicle: ";
		if (actor.hasVehicleNoDamage()) {
			vehCosmeticText = vehCosmeticText + "No damage";
		}
		else {
			vehCosmeticText = vehCosmeticText + "Normal dmg";
		}
		DRAW_TEXT(strdup(vehCosmeticText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_RELATIONSHIP;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	std::string relationshipGroupText = "Group: " + actor.getRelationshipGroup().name;
	DRAW_TEXT(strdup(relationshipGroupText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_SPOT_LIGHT;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	if (actor.hasSpotLight() == true) {
		std::string spotLightText ="Spot light: " + getNameForSpotLightType(actor.getSpotLightType());
		DRAW_TEXT(strdup(spotLightText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	else {
		DRAW_TEXT("Spot light: None", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	if (actor.hasSpotLight() == true) {
		drawIndex++;
		submenu_index++;
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_SPOT_LIGHT_COLOR;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		std::string spotLightColorText = "Spot color: " + actor.getSpotLightColor().name;
		DRAW_TEXT(strdup(spotLightColorText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	}

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_WALK_SPEED;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	if (actor.getWalkingSpeed() == 2.0) {
		DRAW_TEXT("Speed: Run", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}else {
		DRAW_TEXT("Speed: Walk", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_WALK;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	if (actor.hasWalkingStyle()) {
		std::string walkingStyleString = "Walk: " + std::string(actor.getWalkingStyle().name);
		DRAW_TEXT(strdup(walkingStyleString.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	else {
		DRAW_TEXT("Walk: Normal", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_DRIVING_MODE;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	if (actor.isNullActor() ==false) {
		std::string walkingStyleString = "Driving: " + std::string(actor.getDrivingMode().name);
		DRAW_TEXT(strdup(walkingStyleString.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_EDIT_NAME;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	std::string actorName = actor.getName();
	if (actorName.empty()) {
		int actorIndex = get_actor_index_from_ped(actor.getActorPed());
		actorName = "Actor " + std::to_string(actorIndex + 1);
	}
		
	DRAW_TEXT(strdup(("Name: " + actorName).c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	submenu_max_index = submenu_index;
}


void draw_menu_synced_anim_preview() {
	int drawIndex = 0;
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Actor & playerActor = get_actor_from_ped(playerPed);
	submenu_is_displayed = false;

	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
	bool doOutlineText = true;


	if (currentSyncedAnimCategory.empty()) {
		//First add search
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			doOutlineText = false;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
			doOutlineText = true;
		}
		
		DRAW_TEXT("Search", 0.01, 0.108 + (0.04)*drawIndex, 0.3, 0.3, 0, doOutlineText, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.065, 0.120 + (0.04)*drawIndex, 0.123, 0.034, bgColorR, bgColorG, bgColorB, 100);
		
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_SYNCEDPREVIEW_SEARCH;
		}

		drawIndex++;

		//loop through the synced anim categories
		int indexCategory = 0;
		for (auto strCategory : modSyncedAnimCategories) {
			if (menu_active_index == drawIndex) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
				doOutlineText = false;
			}
			else {
				textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
				doOutlineText = true;
			}

			DRAW_TEXT(strdup((strCategory.first + " ("+ std::to_string(strCategory.second) + ")").c_str()), 0.01, 0.108 + (0.04)*drawIndex, 0.3, 0.3, 0, doOutlineText, false, false, false, textColorR, textColorG, textColorB, 200);
			GRAPHICS::DRAW_RECT(0.065, 0.120 + (0.04)*drawIndex, 0.123, 0.034, bgColorR, bgColorG, bgColorB, 100);

			if (menu_active_index == drawIndex) {
				menu_active_action = (MENU_ITEM)(MENU_ITEM_SYNCEDPREVIEW_CATEGORY1 + indexCategory+1);
			}
			drawIndex++;
			indexCategory++;
		}
		menu_max_index = drawIndex - 1;
	}
	else {

		int startIndex= menu_active_index;
		if (startIndex >= 1) {
			startIndex--;
		}

		//loop through the synced animations we have for choosen category
		for (int i = startIndex; i < currentSyncedAnimations.size() && i< startIndex+15; i++) {
			if (menu_active_index == i) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
				doOutlineText = false;
			}
			else {
				textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
				doOutlineText = true;
			}
			DRAW_TEXT(strdup(currentSyncedAnimations[i].getTitle().c_str()), 0.01, 0.108 + (0.04)*drawIndex, 0.3, 0.3, 0, doOutlineText, false, false, false, textColorR, textColorG, textColorB, 200);
			GRAPHICS::DRAW_RECT(0.12, 0.120 + (0.04)*drawIndex, 0.233, 0.034, bgColorR, bgColorG, bgColorB, 100);

			DRAW_TEXT(strdup(("Actors: "+std::to_string(currentSyncedAnimations[i].getNrOfActors())).c_str()), 0.05+0.160, 0.104 + (0.04)*drawIndex, 0.18, 0.18, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			if (!currentSyncedAnimations[i].isProperSynced()) {
				DRAW_TEXT("Manual placement", 0.05 + 0.134, 0.114 + (0.04)*drawIndex, 0.18, 0.18, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			}
			

			if (menu_active_index == i) {
				menu_active_action = (MENU_ITEM)(MENU_ITEM_SYNCEDPREVIEW_ANIMINDEX_START + menu_active_index);;
			}

			drawIndex++;
		}
		//back button as last element
		if (menu_active_index == currentSyncedAnimations.size()) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			doOutlineText = false;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
			doOutlineText = true;
		}
		DRAW_TEXT("<- Back", 0.01, 0.108 + (0.04)*drawIndex, 0.3, 0.3, 0, doOutlineText, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.12, 0.120 + (0.04)*drawIndex, 0.233, 0.034, bgColorR, bgColorG, bgColorB, 100);
		
		if (menu_active_index == currentSyncedAnimations.size()) {
			menu_active_action = MENU_ITEM_SYNCEDPREVIEW_BACK;
		}
		
		drawIndex++;

		//set max index based on the number of synced animations we have for the category
		menu_max_index = currentSyncedAnimations.size();
	}

	if (menu_active_index == -1) {
		menu_active_index = 0;
	}

	if (menu_active_index > menu_max_index) {
		menu_active_index = menu_max_index;
	}

	if (submenu_is_displayed == false) {
		submenu_is_active = false;
		submenu_active_index = -1;
	}
}

void draw_menu() {
	int drawIndex = 0;
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Actor & playerActor = get_actor_from_ped(playerPed);
	submenu_is_displayed = false;

	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB=255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	} else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//DRAW_TEXT("BETA RELEASE OF SCENE DIRECTOR BY ELSEWHAT - NOT FOR DISTRIBUTION", 0.0, 0.0, 0.3, 0.3, 0, false, false, false, false, 255, 255, 255, 155);



	//1. If actor is not assigned to any slot
	if (playerActor.isNullActor()) {
		int available_slot = get_next_free_slot();
		if (available_slot != -1) {
			char* slotText = strdup(("Add actor to slot " + std::to_string(available_slot)).c_str());
			DRAW_TEXT(slotText, 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

			if (menu_active_index == drawIndex) {
				menu_active_action = MENU_ITEM_ADD_TO_SLOT;
			}

			drawIndex++;
			if (menu_active_index == drawIndex) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			}
			else {
				textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
			}

		}

	}


	//2. If clone has just been created
	if (actor0IsClone) {
		int available_slot = get_next_free_slot();
		if (available_slot != -1) {
			char* slotText = strdup(("Add clone to slot " + std::to_string(available_slot)).c_str());
			DRAW_TEXT(slotText, 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

			if (menu_active_index == drawIndex) {
				menu_active_action = MENU_ITEM_ADD_CLONE_TO_SLOT;
			}

			drawIndex++;
			if (menu_active_index == drawIndex) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			}
			else {
				textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
			}

		}
	}


	//3. Scene mode
	if (sceneMode == SCENE_MODE_ACTIVE) {
		DRAW_TEXT("Scene mode: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	} else {
		DRAW_TEXT("Scene mode: Setup", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	}

	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_SCENE_MODE;
	}
	drawIndex++;

	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	} else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//3a. Back to start
	DRAW_TEXT("Back to start", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_BACK_TO_START;
	}
	drawIndex++;

	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//3b. Save / Load
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_SAVE_LOAD;
		draw_submenu_save_load(drawIndex);
		submenu_is_displayed = true;
		if (submenu_active_index == -1) {
			submenu_active_index = 0;
		}
		//dim the main menu selector if sub menu is active
		if (submenu_is_active) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 180, bgColorG = 180, bgColorB = 180;
		}

	}
	DRAW_TEXT("Save / Load", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//show birds eye view
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_BIRDS_EYE_MODE;

	}
	DRAW_TEXT("Edit scene (BETA)", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	//4. World options
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_WORLD;
		draw_submenu_world(drawIndex);
		submenu_is_displayed = true;
		if (submenu_active_index == -1) {
			submenu_active_index = 0;
		}
		//dim the main menu selector if sub menu is active
		if (submenu_is_active) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 180, bgColorG = 180, bgColorB = 180;
		}

	}
	DRAW_TEXT("World / Options", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	drawIndex++;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//show submenu for animation
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_ANIMATION;
		draw_submenu_animation(drawIndex);
		submenu_is_displayed = true;
		if (submenu_active_index == -1) {
			submenu_active_index = 0;
		}
		//dim the main menu selector if sub menu is active
		if (submenu_is_active) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 180, bgColorG = 180, bgColorB = 180;
		}

	}

	DRAW_TEXT("Animation (BETA)", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_ANIMATION;
	}

	drawIndex++;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//4. If autopilot is engaged
	if (is_autopilot_engaged_for_player) {
		DRAW_TEXT("Autopilot: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_AUTOPILOT;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	}

	////5. If chase is engaged
	//if (is_chase_player_engaged) {
	//	DRAW_TEXT("Player chase: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	//	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	//	if (menu_active_index == drawIndex) {
	//		menu_active_action = MENU_ITEM_CHASE;
	//	}

	//	drawIndex++;
	//	if (menu_active_index == drawIndex) {
	//		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	//	}
	//	else {
	//		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	//	}
	//} else if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
	//	DRAW_TEXT("Activate chase", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	//	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	//	if (menu_active_index == drawIndex) {
	//		menu_active_action = MENU_ITEM_CHASE;
	//	}

	//	drawIndex++;
	//	if (menu_active_index == drawIndex) {
	//		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	//	}
	//	else {
	//		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	//	}
	//}


	////6. If escort is engaged
	//if (is_escort_player_engaged) {
	//	DRAW_TEXT("Player escort: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	//	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	//	if (menu_active_index == drawIndex) {
	//		menu_active_action = MENU_ITEM_ESCORT;
	//	}

	//	drawIndex++;
	//	if (menu_active_index == drawIndex) {
	//		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	//	}
	//	else {
	//		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	//	}
	//} else if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
	//	DRAW_TEXT("Activate escort", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	//	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	//	if (menu_active_index == drawIndex) {
	//		menu_active_action = MENU_ITEM_ESCORT;
	//	}

	//	drawIndex++;
	//	if (menu_active_index == drawIndex) {
	//		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	//	}
	//	else {
	//		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	//	}
	//}

	//7. If firing squad is engaged
	if (is_firing_squad_engaged) {
		DRAW_TEXT("Firing squad: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_FIRING_SQUAD;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	}

	//8. Clone actor
	DRAW_TEXT("Clone actor", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_CLONE;
	}

	drawIndex++;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//11. If in vehicle, show option to clone with  vehicle
	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		char* cloneWithVehicleText;
		if (PED::IS_PED_IN_ANY_HELI(playerPed)) {
			cloneWithVehicleText = "Clone actor and heli";
		}
		else if (PED::IS_PED_IN_ANY_PLANE(playerPed)) {
			cloneWithVehicleText = "Clone actor and plane";
		}
		else if (PED::IS_PED_IN_ANY_BOAT(playerPed)) {
			cloneWithVehicleText = "Clone actor and boat";
		}
		else {
			cloneWithVehicleText = "Clone actor and car";
		}

		DRAW_TEXT(cloneWithVehicleText, 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_CLONE_WITH_VEHICLE;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	}

	//10. Possess actor
	DRAW_TEXT("Possess near/aimed", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_POSSESS;
	}

	drawIndex++;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}

	//11. Actors 1-9 if they exist
	for (int i = actors.size()-1; i >= 0; i--)
	{
		if (actors[i].isNullActor() ==false) {
			//check if we should move the selected index to this actor ( menu_active_ped is set when switching to an actor)
			if (menu_active_index == -1 && actors[i].isActorThisPed(playerActor.getActorPed())) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
				menu_active_index = drawIndex;
				menu_active_ped = 0;
			}

			//show submenu for active player
			if (menu_active_index == drawIndex && actors[i].isActorThisPed(playerActor.getActorPed())) {
				submenu_is_displayed = true;
				if (submenu_active_index == -1) {
					submenu_active_index = 0;
				}
				draw_submenu_player(drawIndex);
				//dim the main menu selector if sub menu is active
				if (submenu_is_active) {
					textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 180, bgColorG = 180, bgColorB = 180;
				}
			}

			std::string actorName = actors[i].getName();
			if (actorName.empty()) {
				int actorIndex = get_actor_index_from_ped(actors[i].getActorPed());
				actorName = "Actor " + std::to_string(i + 1);
			}

			char* actorText = strdup((actorName).c_str());
			DRAW_TEXT(actorText, 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

			/* Can't find icon for waypoint, so just using text for now
			if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED("CommonMenu") && actorHasWaypoint[i]) {
				GRAPHICS::DRAW_SPRITE("CommonMenu", "MP_AlertTriangle", 0.95, 0.888 - (0.04)*drawIndex, 0.08, 0.08, 0, 255, 255, 255, 50);
			}*/


			if (actors[i].hasRecording()) {
				DRAW_TEXT("Recording", 0.954, 0.885 - (0.04)*drawIndex, 0.18, 0.18, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			}else if (actors[i].hasWaypoint()) {
				DRAW_TEXT("Waypoint", 0.954, 0.885 - (0.04)*drawIndex, 0.18, 0.18, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			}

			if (actors[i].isActorThisPed(playerActor.getActorPed())) {
				GRAPHICS::DRAW_RECT(0.93, 0.883 - (0.04)*drawIndex, 0.113, 0.002, 100, 255, 0, 100);				
			}


			if (menu_active_index == drawIndex) {
				//i should match value of MENU_ITEM_ACTOR_X
				menu_active_action = (MENU_ITEM)(i+1);
			}



			drawIndex++;
			if (menu_active_index == drawIndex) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			}
			else {
				textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
			}
		}
	}


	if (menu_active_index == -1) {
		menu_active_index = 0;
	}

	menu_max_index = drawIndex - 1;
	if (menu_active_index > menu_max_index) {
		menu_active_index = menu_max_index;
	}

	if (submenu_is_displayed == false) {
		submenu_is_active = false;
		submenu_active_index = -1;
	}

}

void draw_spot_lights() {
	for (auto &actor : actors) {
		if(actor.hasSpotLight()){
			//log_to_file("Drawing spot light for actor with index " + std::to_string(i));
			Vector3 actorPos = ENTITY::GET_ENTITY_COORDS(actor.getActorPed(), true);
			SpotLightColor color = actor.getSpotLightColor();
			int colorR = color.r;
			int colorG = color.g;
			int colorB = color.b;

			
			switch (actor.getSpotLightType()) {
			case SPOT_LIGHT_NONE:
				break;
			case SPOT_LIGHT_ACTOR_ABOVE:
				GRAPHICS::DRAW_SPOT_LIGHT(actorPos.x, actorPos.y, actorPos.z + 20.0f, 0, 0, -1.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 4.0f, 1.0f);
				break;
			case SPOT_LIGHT_WEST:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x+10.0f, actorPos.y, actorPos.z, -1.0, 0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			case SPOT_LIGHT_EAST:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x-10.0f, actorPos.y, actorPos.z, 1.0, 0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			case SPOT_LIGHT_NORTH:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x, actorPos.y - 10.0f, actorPos.z, 0.0, 1.0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			case SPOT_LIGHT_SOUTH:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x, actorPos.y+10.0f, actorPos.z, 0.0, -1.0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			default:
				break;
			}

			
		}
	}
}


bool move_to_waypoint(Ped ped, Vector3 waypointCoord, bool suppress_msgs) {
	log_to_file("move_to_waypoint: Ped:" + std::to_string(ped) + " x:" + std::to_string(waypointCoord.x) + " y : " + std::to_string(waypointCoord.y) + " z : " + std::to_string(waypointCoord.z));

	Actor & actor = get_actor_from_ped(ped);
	//code inspired by LUA plugin https://www.gta5-mods.com/scripts/realistic-vehicle-controls
	if (actor.isNullActor()== false && actor.isActiveInScene()) {

		if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);

			//check if player is the driver
			Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
			if (pedDriver != ped) {
				log_to_file("move_to_waypoint: Ped (" + std::to_string(ped)+ " is not driver (" + std::to_string(pedDriver));
				//set_status_text("Ped is not driver. Ignore waypoint");
				return false;
			}
			else {
				actor.setHasWaypoint(true);
				actor.setWaypoint(waypointCoord);

				float vehicleMaxSpeed = VEHICLE::_GET_VEHICLE_MAX_SPEED(ENTITY::GET_ENTITY_MODEL(pedVehicle));

				if (PED::IS_PED_IN_ANY_HELI(ped)) {
					int height_above_ground = GetPrivateProfileInt("waypoint", "heli_waypoint_height_above_ground", 50, config_path);

					AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z+height_above_ground, vehicleMaxSpeed, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
					log_to_file("move_to_waypoint: Flying in heli with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed) + " height above ground " + std::to_string(height_above_ground));
					if (suppress_msgs != true) {
						set_status_text("Flying to waypoint");
					}
				}
				else if (PED::IS_PED_IN_ANY_PLANE(ped)) {
					int height_above_ground = GetPrivateProfileInt("waypoint", "plane_waypoint_height_above_ground",50, config_path);
					//z dimension is on ground level so add a bit to it
					AI::TASK_PLANE_MISSION(pedDriver, pedVehicle, 0, 0, waypointCoord.x, waypointCoord.y, waypointCoord.z + height_above_ground, 4, 30.0, 50.0, -1, vehicleMaxSpeed, 50);

					//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, 500.0, vehicleMaxSpeed, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
					log_to_file("move_to_waypoint: Flying in plane with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed) + " height above ground " + std::to_string(height_above_ground));
					if (suppress_msgs != true) {
						set_status_text("Flying to waypoint");
					}
				}else {
					//AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, VEHICLE::_GET_VEHICLE_MAX_SPEED(pedVehicle), 786469, 50.0);
					float speed = vehicleMaxSpeed; 
					if (actor.getDrivingMode().useVehicleMaxSpeed == false) {
						speed = actor.getDrivingMode().manualMaxSpeed;
					}
					AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, speed, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), actor.getDrivingMode().value,5.0, -1);
					log_to_file("move_to_waypoint: Driving with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(speed));
					if (suppress_msgs != true) {
						set_status_text("Driving to waypoint");
					}

				}
				//initial: Quite aggressive, but stops for redlight (I think)
				//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, 100, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
				//slow and follows rules
				//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, 15.0, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786599, 4.0, -1.0);

				//aggresive and drives on redlights
				//AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(pedDriver, playerVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, VEHICLE::_GET_VEHICLE_MAX_SPEED(playerVehicle), 786469, 50.0);

			}
			
			return true;
		}
		else if (PED::IS_PED_ON_FOOT(ped)) {
			AI::TASK_GO_STRAIGHT_TO_COORD(ped, waypointCoord.x, waypointCoord.y, waypointCoord.z, actor.getWalkingSpeed(), -1, 27.0f, 0.5f);
			log_to_file("move_to_waypoint: Ped (" + std::to_string(ped) + " is walking to waypoint");
			if (suppress_msgs != true) {
				set_status_text("Walking to waypoint");
			}
			return true;

		}
		return true;
	}
	else {//scene is passive
		return false;
	}

}

void playback_recording_to_waypoint(Ped ped, Vector3 waypointCoord) {
	log_to_file("playback_recording_to_waypoint: Ped:" + std::to_string(ped) + " x:" + std::to_string(waypointCoord.x) + " y : " + std::to_string(waypointCoord.y) + " z : " + std::to_string(waypointCoord.z));
	Actor & actor = get_actor_from_ped(ped);

	if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);

		//check if player is the driver
		Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
		if (pedDriver != ped) {
			log_to_file("move_to_waypoint: Ped (" + std::to_string(ped) + " is not driver (" + std::to_string(pedDriver));
			//set_status_text("Ped is not driver. Ignore waypoint");
		}
		else {
			float vehicleMaxSpeed = VEHICLE::_GET_VEHICLE_MAX_SPEED(ENTITY::GET_ENTITY_MODEL(pedVehicle));

			if (PED::IS_PED_IN_ANY_HELI(ped)) {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, vehicleMaxSpeed, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
				log_to_file("playback_recording_to_waypoint: Flying in heli with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed) );
			}
			else if (PED::IS_PED_IN_ANY_PLANE(ped)) {
				AI::TASK_PLANE_MISSION(pedDriver, pedVehicle, 0, 0, waypointCoord.x, waypointCoord.y, waypointCoord.z, 4, 30.0, 50.0, -1, vehicleMaxSpeed, 50);
				log_to_file("playback_recording_to_waypoint: Flying in plane with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed) );

			}
			else if (PED::IS_PED_IN_ANY_BOAT(ped)) {
				AI::TASK_BOAT_MISSION(pedDriver, pedVehicle, 0, 0, waypointCoord.x, waypointCoord.y, waypointCoord.z, 4, vehicleMaxSpeed, 786469, -1.0, 7);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(pedDriver, 1);
				//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, 0.0, 20.0, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786469, 5.0, 1071);
				log_to_file("playback_recording_to_waypoint: In boat : " + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));
			}
			else {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, vehicleMaxSpeed, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), actor.getDrivingMode().value, 5.0, -1);
				log_to_file("playback_recording_to_waypoint: Driving with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));

			}
		}

	}
	else if (PED::IS_PED_ON_FOOT(ped)) {
		AI::TASK_GO_STRAIGHT_TO_COORD(ped, waypointCoord.x, waypointCoord.y, waypointCoord.z, 1.0f, -1, 27.0f, 0.5f);
		log_to_file("playback_recording_to_waypoint: Ped (" + std::to_string(ped) + " is walking to waypoint");
	}
}

void teleport_entity_to_location(Entity entityToTeleport, Vector3 location, bool trustZValue) {
	log_to_file("teleport_entity_to_location: Entity:" + std::to_string(entityToTeleport));
	//From the native trainer. Could it be replaced with PATHFIND::GET_SAFE_COORD_FOR_PED ?

	if (trustZValue == false) {
		// load needed map region and check height levels for ground existence
		bool groundFound = false;
		static float groundCheckHeight[] = {
			100.0, 150.0, 50.0, 0.0, 200.0, 250.0, 300.0, 350.0, 400.0,
			450.0, 500.0, 550.0, 600.0, 650.0, 700.0, 750.0, 800.0
		};
		for (int i = 0; i < sizeof(groundCheckHeight) / sizeof(float); i++)
		{
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entityToTeleport, location.x, location.y, groundCheckHeight[i], 0, 0, 1);
			WAIT(100);
			if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, groundCheckHeight[i], &location.z,0))
			{
				groundFound = true;
				location.z += 3.0;
				break;
			}
		}
		// if ground not found then set Z in air and give player a parachute
		if (!groundFound)
		{
			location.z = 1000.0;
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), 0xFBAB5776, 1, 0);
		}
	}

	ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entityToTeleport, location.x, location.y, location.z, 0, 0, 1);

	//after a teleport, actions some time seems to be stuck
	if (ENTITY::IS_ENTITY_A_PED(entityToTeleport)) {
		//AI::CLEAR_PED_TASKS(entityToTeleport);
	}
	else if (ENTITY::IS_ENTITY_A_VEHICLE(entityToTeleport)) {
		VEHICLE::SET_VEHICLE_ENGINE_ON(entityToTeleport, false, true,false);
		Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(entityToTeleport, -1);
		if (pedDriver >= 1) {
			//AI::CLEAR_PED_TASKS(pedDriver);
			//AI::TASK_PAUSE(pedDriver, 500);
		}
	}

	set_status_text("Teleporting to waypoint");

	nextWaitTicks = 500;
}

void teleport_player_to_waypoint() {
	log_to_file("teleport_player_to_waypoint");
	// get entity to teleport
	Entity entityToTeleport = PLAYER::PLAYER_PED_ID();

	if (PED::IS_PED_IN_ANY_VEHICLE(entityToTeleport, 0)) {
		entityToTeleport = PED::GET_VEHICLE_PED_IS_USING(entityToTeleport);


	}

	if (UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);

		teleport_entity_to_location(entityToTeleport, waypointCoord,false);
		if (VEHICLE::IS_VEHICLE_STUCK_ON_ROOF(entityToTeleport)) {
			VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(entityToTeleport);
			WAIT(100);
		}
	}
	else {
		set_status_text("Set waypoint before teleporting");
	}

	nextWaitTicks = 500;
}



void possess_ped(Ped swapToPed) {
	log_to_file("possess_ped: Ped:" + std::to_string(swapToPed));
	if (ENTITY::DOES_ENTITY_EXIST(swapToPed) && ENTITY::IS_ENTITY_A_PED(swapToPed) && !ENTITY::IS_ENTITY_DEAD(swapToPed)) {
		Ped swapFromPed = PLAYER::PLAYER_PED_ID();

		if (swapFromPed == swapToPed) {
			if (is_autopilot_engaged_for_player == true) {
				AI::CLEAR_PED_TASKS(swapFromPed);
				is_autopilot_engaged_for_player = false;
				set_status_text("Autopilot disengaged. ALT+A to start it again");
				log_to_file("possess_ped: Autopilot disengaged");
				nextWaitTicks = 200;
			}
			return;
		}

		//check if the ped where swapping from should continue towards a waypoint
		Actor & actor = get_actor_from_ped(swapFromPed);
		if (actor.isNullActor()==false && actor.hasWaypoint()) {
			move_to_waypoint(swapFromPed, actor.getWaypoint(),true);
		}
		else if (UI::IS_WAYPOINT_ACTIVE()) {
			int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
			Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
			move_to_waypoint(swapFromPed, waypointCoord, true);
		}


		PLAYER::CHANGE_PLAYER_PED(PLAYER::PLAYER_ID(), swapToPed, false, false);

		//stop any animations or scenarios being run on the ped
		AI::CLEAR_PED_TASKS(swapToPed);
		is_autopilot_engaged_for_player = false;

		previousActor = actor;
		actor0IsClone = false;
	}
	else {
		log_to_file("possess_ped: Could not possess ped (entity may be probably dead) Ped:" + std::to_string(swapToPed));
		set_status_text("Could not possess ped");
	}
	nextWaitTicks = 500;

}




void action_possess_ped() {
	log_to_file("action_possess_ped");
	Player player = PLAYER::PLAYER_ID();
	nextWaitTicks = 300;

	//If player is aiming at a ped, control that
	if (PLAYER::IS_PLAYER_FREE_AIMING(player)) {
		Entity targetEntity;
		PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &targetEntity);

		if (ENTITY::DOES_ENTITY_EXIST(targetEntity) && ENTITY::IS_ENTITY_A_PED(targetEntity)) {

			if (ENTITY::IS_ENTITY_DEAD(targetEntity)) {
				set_status_text("Dead actors can't act");
				return;
			}

			set_status_text("Possessing targeted pedestrian");
			log_to_file("action_possess_ped: Possessing target ped " + std::to_string(targetEntity));

			possess_ped(targetEntity);
			//give a pistol, so they can easily target the next person
			give_basic_weapon(targetEntity);

		}
		else {
			set_status_text("Aim at a pedestrian and try again");
		}
	}
	else {//if not, take control of the closest one
		//inspired by https://github.com/Reck1501/GTAV-EnhancedNativeTrainer/blob/aea568cd1a7134c0a9adf3e9dc1fd7f4640d0a1c/EnhancedNativeTrainer/script.cpp#L1104
		const int numElements = 1;
		const int arrSize = numElements * 2 + 2;

		Ped *peds = new Ped[arrSize];
		peds[0] = numElements;
		int nrPeds = PED::GET_PED_NEARBY_PEDS(PLAYER::PLAYER_PED_ID(), peds, -1);
		for (int i = 0; i < nrPeds; i++)
		{
			int offsettedID = i * 2 + 2;

			if (!ENTITY::DOES_ENTITY_EXIST(peds[offsettedID]) || ENTITY::IS_ENTITY_DEAD(peds[offsettedID]))
			{
				continue;
			}

			Ped closestPed = peds[offsettedID];
			set_status_text("Possessing closest pedestrian");
			log_to_file("action_possess_ped: Possessing target ped " + std::to_string(closestPed));

			possess_ped(closestPed);

		}
		delete peds;
	}
}




void action_clone_player() {
	log_to_file("action_clone_player");
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	Ped clonedPed = PED::CLONE_PED(playerPed, 0.0f, false, true);
	previousActor = clonedPed;
	actor0IsClone = true;

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		PED::SET_PED_INTO_VEHICLE(clonedPed, vehicle, -2);
	}
	else {//clone a bit ahead of the player and facing towards you
		ENTITY::SET_ENTITY_HEADING(clonedPed, ENTITY::GET_ENTITY_HEADING(playerPed)+180.0f);
		teleport_entity_to_location(clonedPed, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 1.0, 3.0, 0.0), false);
	}

	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(clonedPed, true);
	//no longer assign to default rel group
	//assign_actor_to_relationship_group(clonedPed,getDefaultRelationshipGroup());

	set_status_text("Cloned player");
	menu_active_index = 0;
	nextWaitTicks = 500;
}

void action_clone_player_with_vehicle() {
	log_to_file("action_clone_player_with_vehicle");
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{

		Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);


		//location of the clone vehicle
		Vector3 clonedVehicleCoords;

		//for planes and helis we check if we're on the ground or in air
		if (PED::IS_PED_IN_ANY_HELI(playerPed) || PED::IS_PED_IN_ANY_PLANE(playerPed)) {
			float zAboveGround = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(playerVehicle);
			log_to_file("ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(playerVehicle) " + std::to_string(zAboveGround));

			if (zAboveGround > 3.0 && PED::IS_PED_IN_ANY_HELI(playerPed)) {//in air heli
				//30.0 in front and 25.0 above
				clonedVehicleCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.0, 30.0, 25.0);
			}
			else if (zAboveGround > 3.0 && PED::IS_PED_IN_ANY_PLANE(playerPed)) {//in air plane
			 //30.0 in front and 100.0 above
				clonedVehicleCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.0, 30.0, 200.0);
			}
			else {//on ground
				clonedVehicleCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.0, 15.0, 0.0);
			}
		}
		else {
			clonedVehicleCoords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 0.0, 5.0, 0.0);
		}


		DWORD model = ENTITY::GET_ENTITY_MODEL(playerVehicle);
		//let's make sure it is loaded (should be already)
		STREAMING::REQUEST_MODEL(model);
		while (!STREAMING::HAS_MODEL_LOADED(model)) WAIT(0);

		//clone the vehicle (doesn't include mods etc)
		Vehicle clonedVehicle = VEHICLE::CREATE_VEHICLE(model, clonedVehicleCoords.x, clonedVehicleCoords.y, clonedVehicleCoords.z, 0.0, 1, 1);
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(clonedVehicle);

		ENTITY::SET_ENTITY_HEADING(clonedVehicle, ENTITY::GET_ENTITY_HEADING(playerPed));

		//clone the player and assign into car
		Ped clonedPed = PED::CLONE_PED(playerPed, 0.0f, false, true);

		previousActor = clonedPed;
		actor0IsClone = true;

		//try to prevent fleeing during teleport
		AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(clonedPed, true);
		PED::SET_PED_FLEE_ATTRIBUTES(clonedPed, 0, 0);

		PED::SET_PED_INTO_VEHICLE(clonedPed, clonedVehicle, -1);

		AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(clonedPed, false);

		nextWaitTicks = 300;
	}
	else {
		set_status_text("Clone with vehicle requires that you're in an actual vehicle");
	}

}


void action_enter_nearest_vehicle_as_passenger() {
	log_to_file("enter_nearest_vehicle_as_passenger");
	set_status_text("Entering as passenger");
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 coord = ENTITY::GET_ENTITY_COORDS(playerPed, true);

	Vehicle vehicle = VEHICLE::GET_CLOSEST_VEHICLE(coord.x, coord.y, coord.z, 100.0, 0, 71);
	int nrOfSeats = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vehicle);

	log_to_file("Vehicle has " + std::to_string(nrOfSeats) + " seats");

	int seat = -2;
	for (int i = 0; i < nrOfSeats; i++) {
		if (VEHICLE::IS_VEHICLE_SEAT_FREE(vehicle, i)) {
			seat = i;
			break;
		}
	}
	log_to_file("Seat is " + std::to_string(seat));

	if (seat == -2 && nrOfSeats==0 && (PED::IS_PED_IN_ANY_PLANE(playerPed) || PED::IS_PED_IN_ANY_HELI(playerPed))){
		log_to_file("Teleporting ped to heli or plane since it has zero seats");
		PED::SET_PED_INTO_VEHICLE(playerPed, vehicle, -2);
	} else {
		AI::TASK_ENTER_VEHICLE(playerPed, vehicle, -1, seat, 1.0, 1, 0);
	}

	nextWaitTicks = 200;
}

void action_explode_nearby_vehicles(boolean setOutOfControl) {
	log_to_file("action_explode_nearby_vehicle");
	
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerLocation = ENTITY::GET_ENTITY_COORDS(playerPed, true);
	Vector3 sphereCenter = ENTITY::GET_ENTITY_COORDS(playerPed, true);

	Vehicle pedVehicle = 0;
	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	}

	std::map <Vehicle, Vehicle> nearbyVehicles;
	Vehicle nearbyVehicle; 

	float startHeading = ENTITY::GET_ENTITY_HEADING(playerPed);
	log_to_file("Start heading is " + std::to_string(startHeading));

	for (int i = 0; i < 360; i+=20) {
		float offsetX = (float)sin((i *PI / 180.0f))*15.0f;
		float offsetY = (float)cos((i *PI / 180.0f))*15.0f;

		//log_to_file("Offsetx " + std::to_string(offsetX));
		//log_to_file("Offsety " + std::to_string(offsetY));

		//nearbyVehicle = VEHICLE::GET_RANDOM_VEHICLE_IN_SPHERE(playerLocation.x+offsetX, playerLocation.y+offsetY, playerLocation.z, 20.0, 0, 0);
		nearbyVehicle = VEHICLE::GET_CLOSEST_VEHICLE(playerLocation.x + offsetX, playerLocation.y + offsetY, playerLocation.z, 15.0, 0, 1|2|4|8|16|32|64|2048|65536);

		log_to_file("Random vehicle is " + std::to_string(nearbyVehicle));
		if (nearbyVehicle == pedVehicle) {
			continue;
		}
		else {
			if (nearbyVehicles.find(nearbyVehicle) == nearbyVehicles.end())
			{
				nearbyVehicles[nearbyVehicle] = nearbyVehicle;
			}
		}
	}


	for (auto const &tupleNearbyVehicle : nearbyVehicles) {
		Vehicle targetVehicle = tupleNearbyVehicle.first;
		
		bool actorsInVehicle = false;
		log_to_file("Found vehicle " + std::to_string(targetVehicle));


		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);

			if (pedVehicle == targetVehicle) {
				actorsInVehicle = true;
			}
		}

		for (auto &actor : actors) {
			if (actor.isNullActor() == false && actor.isActorInVehicle(targetVehicle) == true) {
				actorsInVehicle = true;
			}
		}
		if (!actorsInVehicle) {
			if (setOutOfControl) {
				VEHICLE::SET_VEHICLE_OUT_OF_CONTROL(targetVehicle, true, true);
			}
			else {
				VEHICLE::EXPLODE_VEHICLE(targetVehicle, true, false);
			}
		}
	}

	nextWaitTicks = 200;
}

void check_if_ped_is_passenger_and_has_waypoint(Ped ped) {
	if (is_ped_actor_active(ped)) {

		if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);

			//check if player is a passenger
			Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);

			if (pedDriver != ped) {
				//player is a passenger, check if player has a waypoint
				if (UI::IS_WAYPOINT_ACTIVE()) {
					int waypointID;
					Vector3 waypointCoord;

					waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
					if (waypointID != lastWaypointID) {
						log_to_file("check_if_ped_is_passenger_and_has_waypoint: New waypoint from passenger");

						waypointCoord = UI::GET_BLIP_COORDS(waypointID);
						lastWaypointID = waypointID;

						set_status_text("Driving to passenger waypoint");
						move_to_waypoint(pedDriver, waypointCoord, true);

						nextWaitTicks = 200;

						Actor & actor = get_actor_from_ped(pedDriver);
						if (actor.isNullActor() == false) {
							actor.setHasWaypoint(true);
							actor.setWaypoint(waypointCoord);
						}

					}
				}
			}
		}
	}

}

void check_if_player_is_passenger_and_has_waypoint() {
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	check_if_ped_is_passenger_and_has_waypoint(playerPed);
}

/*
void action_increase_aggressiveness(Ped ped, bool suppress_msgs) {
	log_to_file("action_increase_aggressiveness");
	int actorIndex = get_index_for_actor(ped);
	if (actorIndex != -1) {
		float currentAggressiveness = actorDriverAgressiveness[actorIndex];
		if (currentAggressiveness >= 1.0f) {
			if (suppress_msgs == false) {
				set_status_text("Already at max aggression");
			}
			return;
		}

		currentAggressiveness = currentAggressiveness + 0.1f;
		if (currentAggressiveness > 1.0f)
		{
			currentAggressiveness = 1.0f;
		}

		PED::SET_DRIVER_AGGRESSIVENESS(ped, currentAggressiveness);
		//let's increase ability as well, just for show
		PED::SET_DRIVER_ABILITY(ped, currentAggressiveness);

		actorDriverAgressiveness[actorIndex] = currentAggressiveness;
		if (suppress_msgs == false) {
			set_status_text("Increasing driver aggressiveness " + std::to_string(currentAggressiveness));
		}
	}
	else {
		set_status_text("Actor must be stored in a slot to increase aggressiveness");
	}
	nextWaitTicks = 200;
}*/
/*
void action_decrease_aggressiveness(Ped ped, bool suppress_msgs) {
	log_to_file("action_decrease_aggressiveness");
	int actorIndex = get_index_for_actor(ped);
	if (actorIndex != -1) {
		float currentAggressiveness = actorDriverAgressiveness[actorIndex];

		if (currentAggressiveness <= 0.0f) {
			if (suppress_msgs == false) {
				set_status_text("Already at min aggression");
			}
			return;
		}


		currentAggressiveness = currentAggressiveness - 0.1f;
		if (currentAggressiveness < 0.0f)
		{
			currentAggressiveness = 0.0f;
		}

		PED::SET_DRIVER_AGGRESSIVENESS(ped, currentAggressiveness);
		//let's increase ability as well, just for show
		PED::SET_DRIVER_ABILITY(ped, currentAggressiveness);

		actorDriverAgressiveness[actorIndex] = currentAggressiveness;
		if (suppress_msgs == false) {
			set_status_text("Decreasing driver aggressiveness to " + std::to_string(currentAggressiveness));
		}
	}
	else {
		set_status_text("Actor must be stored in a slot to increase aggressiveness");
	}
	nextWaitTicks = 200;
}
*/

/*
void action_increase_aggressiveness_for_all_actors() {
	log_to_file("action_increase_aggressiveness_for_all_actors");
	set_status_text("Increasing aggressiveness for all actors");
	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		action_increase_aggressiveness(actorShortcut[i], true);
	}
}

void action_decrease_aggressiveness_for_all_actors() {
	log_to_file("action_decrease_aggressiveness_for_all_actors");
	set_status_text("Decreasing aggressiveness for all actors");
	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		action_decrease_aggressiveness(actorShortcut[i], true);
	}
}
*/


bool swap_to_previous_possessed_key_pressed()
{
	//see https://msdn.microsoft.com/de-de/library/windows/desktop/dd375731(v=vs.85).aspx for key codes	
	return IsKeyDown(VK_CONTROL) && IsKeyDown(0x30);
}

void add_ped_to_slot(int slotIndex, Ped ped) {
	//check if there exist an actor for this index
	if (actors[slotIndex-1].isNullActor() ==false) {
		if (forceSlotIndexOverWrite != slotIndex) {
			log_to_file("action_if_ped_assign_shortcut_key_pressed: Slot already exists");
			set_status_text("Actor already exist in slot. CTRL+" + std::to_string(slotIndex) + " once more to overwrite");
			forceSlotIndexOverWrite = slotIndex;
			return;
		}
		else {
			log_to_file("action_if_ped_assign_shortcut_key_pressed: Slot will be overwritten");
			forceSlotIndexOverWrite = -1;
			
			//Remove old blip
			int blipIdsToRemove[1] = { actors[slotIndex - 1].getBlipId() };
			UI::REMOVE_BLIP(blipIdsToRemove);
			//and continue storing the actor
		}

	}
	actors.at(slotIndex - 1) = Actor(ped);

	//Disable default assignment to a rel group
	//assign_actor_to_relationship_group(ped, getDefaultRelationshipGroup());

	int blipId = UI::ADD_BLIP_FOR_ENTITY(ped);
	actors[slotIndex - 1].setBlipId(blipId);
	//BLIP Sprite for nr1=17, nr9=25
	UI::SET_BLIP_SPRITE(blipId, 16 + slotIndex);

	//Store current waypoint
	if (UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		actors[slotIndex-1].setWaypoint(waypointCoord);
		actors[slotIndex - 1].setHasWaypoint(true);
	}
	actors[slotIndex - 1].setName("Actor " + std::to_string(slotIndex));

	ensure_ped_and_vehicle_is_not_deleted(ped);

	ensure_max_driving_ability(ped);

}

void action_if_ped_assign_shortcut_key_pressed()
{
	//ALT key
	if (IsKeyDown(VK_CONTROL)) {
		int pedShortcutsIndex = -1;
		//Valid keys - 31=1 39=9
		if (IsKeyDown(0x31)) {
			//0 index is reserved for previously possessed or cloned
			pedShortcutsIndex = 1;
		}
		else if (IsKeyDown(0x32)) {
			pedShortcutsIndex = 2;
		}
		else if (IsKeyDown(0x33)) {
			pedShortcutsIndex = 3;
		}
		else if (IsKeyDown(0x34)) {
			pedShortcutsIndex = 4;
		}
		else if (IsKeyDown(0x35)) {
			pedShortcutsIndex = 5;
		}
		else if (IsKeyDown(0x36)) {
			pedShortcutsIndex = 6;
		}
		else if (IsKeyDown(0x37)) {
			pedShortcutsIndex = 7;
		}
		else if (IsKeyDown(0x38)) {
			pedShortcutsIndex = 8;
		}
		else if (IsKeyDown(0x39)) {
			pedShortcutsIndex = 9;
		}

		if (pedShortcutsIndex != -1) {
			nextWaitTicks = 250;

			log_to_file("action_if_ped_assign_shortcut_key_pressed");

			add_ped_to_slot(pedShortcutsIndex, PLAYER::PLAYER_PED_ID());
			
			log_to_file("action_if_ped_assign_shortcut_key_pressed: Stored current ped in slot " + std::to_string(pedShortcutsIndex));
			set_status_text("Stored current ped. Retrieve with ALT+" + std::to_string(pedShortcutsIndex));

			should_display_hud = true;

		}
	}
}


void action_remove_actor_from_index(int pedShortcutsIndex) {
	if (actors[pedShortcutsIndex-1].isNullActor()) {
		set_status_text("No stored actor in slot " + std::to_string(pedShortcutsIndex));
	}
	else {
		//remove blip
		int blipIdsToRemove[1] = { actors[pedShortcutsIndex - 1].getBlipId() };
		UI::REMOVE_BLIP(blipIdsToRemove);

		actors.at(pedShortcutsIndex - 1) = Actor::nullActor();
	}
}

void action_swap_to_actor_with_index(int pedShortcutsIndex) {
	if (actors[pedShortcutsIndex-1].isNullActor()) {
		set_status_text("No stored actor. Store with CTRL+" + std::to_string(pedShortcutsIndex));
	}
	else {
		log_to_file("action_if_ped_execute_shortcut_key_pressed: Retrieve ped in slot " + std::to_string(pedShortcutsIndex));
		Actor actor = actors[pedShortcutsIndex - 1];
		Ped actorPed = actor.getActorPed();
		if (!ENTITY::DOES_ENTITY_EXIST(actorPed)) {
			set_status_text("Actor does not exist anymore. Removing it");
			action_remove_actor_from_index(pedShortcutsIndex );
		}else if (ENTITY::IS_ENTITY_DEAD(actorPed)) {
			log_to_file("action_if_ped_execute_shortcut_key_pressed: Dead ped in slot " + std::to_string(pedShortcutsIndex));
			set_status_text("Thou shalt not swap to a dead actor");
		}
		else {
			log_to_file("action_if_ped_execute_shortcut_key_pressed: Switching to ped:  " + std::to_string(actorPed));

			//first store the waypoint for the current actor
			store_current_waypoint_for_actor(actorPed);

			possess_ped(actorPed);
		}
	}
}

void action_if_ped_execute_shortcut_key_pressed()
{
	//ALT key
	if (IsKeyDown(VK_MENU)) {
		int pedShortcutsIndex = -1;
		//Valid keys - 31=1 39=9
		if (IsKeyDown(0x30)) {
			//0 index cannot be assigned, but is previously possessed or cloned
			pedShortcutsIndex = 0;
		}
		else if (IsKeyDown(0x31)) {
			pedShortcutsIndex = 1;
		}
		else if (IsKeyDown(0x31)) {
			pedShortcutsIndex = 1;
		}
		else if (IsKeyDown(0x32)) {
			pedShortcutsIndex = 2;
		}
		else if (IsKeyDown(0x33)) {
			pedShortcutsIndex = 3;
		}
		else if (IsKeyDown(0x34)) {
			pedShortcutsIndex = 4;
		}
		else if (IsKeyDown(0x35)) {
			pedShortcutsIndex = 5;
		}
		else if (IsKeyDown(0x36)) {
			pedShortcutsIndex = 6;
		}
		else if (IsKeyDown(0x37)) {
			pedShortcutsIndex = 7;
		}
		else if (IsKeyDown(0x38)) {
			pedShortcutsIndex = 8;
		}
		else if (IsKeyDown(0x39)) {
			pedShortcutsIndex = 9;
		}

		if (pedShortcutsIndex != -1) {
			//TODO: Check if it exist
			nextWaitTicks = 300;

			action_swap_to_actor_with_index(pedShortcutsIndex);
		}
	}
}



void action_reset_scene_director() {
	set_status_text("Resetting scene director to initial status");

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	for (auto &actor : actors) {
		if (actor.isActorThisPed(playerPed) == false) {
			Ped ped = actor.getActorPed();

			if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {
				Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);
				ENTITY::SET_ENTITY_AS_MISSION_ENTITY(playerVehicle, false, false);
				ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&playerVehicle);
			}
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ped, false, false);
			ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&ped);
		}

		int blipIdsToRemove[1] = { actor.getBlipId() };
		UI::REMOVE_BLIP(blipIdsToRemove);

		actor = Actor::nullActor();
	}

}

void action_save_actors() {
	log_to_file("action_save_actors");
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLNode* rootElement = doc->InsertEndChild(doc->NewElement("SceneDirector"));

	for (auto &actor : actors) {
		if (!actor.isNullActor()) {
			Ped actorPed = actor.getActorPed();

			tinyxml2::XMLElement* actorElement = doc->NewElement("Actor");

			Hash pedModelHash = ENTITY::GET_ENTITY_MODEL(actorPed);
			
			log_to_file("Actor pedModelHash=" + std::to_string(pedModelHash));

			char strHash[10];
			sprintf(strHash, "%lu", pedModelHash);
			actorElement->SetAttribute("pedModelHash", strHash);
			//actorElement->SetAttribute("pedModelHash", std::to_string(pedModelHash).c_str());

			for (int i = 0; i <= 11; i++) {
				int drawableVar = PED::GET_PED_DRAWABLE_VARIATION(actorPed, i);
				int textureVar = PED::GET_PED_TEXTURE_VARIATION(actorPed, i);
				int paletteVar = PED::GET_PED_PALETTE_VARIATION(actorPed, i);

				actorElement->SetAttribute(("drawableVariation"+std::to_string(i)).c_str(), drawableVar);
				actorElement->SetAttribute(("textureVariation" +std::to_string(i)).c_str(), textureVar);
				actorElement->SetAttribute(("paletteVariation" + std::to_string(i)).c_str(), paletteVar);

			}	

			for (int i = 0; i <= 2; i++) {
				int propVar = PED::GET_PED_PROP_INDEX(actorPed, i);
				int propTextureVar = PED::GET_PED_PROP_TEXTURE_INDEX(actorPed, i);

				actorElement->SetAttribute(("propVariation" + std::to_string(i)).c_str(), propVar);
				actorElement->SetAttribute(("propTextureVariation" + std::to_string(i)).c_str(), propTextureVar);
			}

			rootElement->InsertEndChild(actorElement);
		}
	}

	log_to_file("action_save_actors: about to save");
	
	doc->SaveFile(saveFileName.c_str());
	set_status_text("Saved actors to " + saveFileName);

	delete doc;
}

void action_load_actors() {
	log_to_file("action_load_actors");
	
	tinyxml2::XMLDocument doc = new tinyxml2::XMLDocument();
	doc.LoadFile(saveFileName.c_str());
	if (doc.Error()) {
		set_status_text("Save file " + saveFileName + " could not be loaded. Error: "+ doc.ErrorName() );
		return;
	}
	
	tinyxml2::XMLElement* sceneDirectorElement = doc.RootElement();
	int actorIndex = 0;
	for (tinyxml2::XMLElement* actorElement = sceneDirectorElement->FirstChildElement("Actor");
		actorElement;
		actorElement = actorElement->NextSiblingElement())
	{
		const char* strPedModelHash = actorElement->Attribute("pedModelHash");
		log_to_file("Actor strPedModelHash=" + std::string(strPedModelHash));
		DWORD pedModelHash = strtoul(strPedModelHash, NULL, 0);
		log_to_file("Actor pedModelHash=" + std::to_string(pedModelHash));



		STREAMING::REQUEST_MODEL(pedModelHash);
		while (!STREAMING::HAS_MODEL_LOADED(pedModelHash)) {
			WAIT(0);
		}

		Vector3 location = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
		float startHeading = ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID());

		Ped newActorPed = PED::CREATE_PED(4, pedModelHash, location.x, location.y, location.z, startHeading, false, true);

		log_to_file("Setting ped variant");

		//set variant of ped
		for (int i = 0; i < 12; i++) {
			const char* strDrawableVar = actorElement->Attribute(("drawableVariation" + std::to_string(i)).c_str());
			const char* strTextureVar = actorElement->Attribute(("textureVariation" + std::to_string(i)).c_str());
			const char* strPaletteVar = actorElement->Attribute(("paletteVariation" + std::to_string(i)).c_str());

			int drawableVar = std::stoi(strDrawableVar);
			int textureVar = std::stoi(strTextureVar);
			int paletteVar = std::stoi(strPaletteVar);
				
			PED::SET_PED_COMPONENT_VARIATION(newActorPed, i, drawableVar, textureVar, paletteVar);
		}

		log_to_file("Setting ped props");
		//get props
		for (int i = 0; i <= 2; i++) {
			const char* strPropVar = actorElement->Attribute(("propVariation" + std::to_string(i)).c_str());
			const char* strPropTextureVar = actorElement->Attribute(("propTextureVariation" + std::to_string(i)).c_str());

			int propVar = std::stoi(strPropVar);
			int propTextureVar = std::stoi(strPropTextureVar);

			PED::SET_PED_PROP_INDEX(newActorPed, i, propVar, propTextureVar, 2);
		}

		//to force overwrite of any existing actor in ad_ped_to_slot
		forceSlotIndexOverWrite = actorIndex+1;
		//assign actor
		add_ped_to_slot(actorIndex+1, newActorPed);

		//clear model from mem
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(pedModelHash);

		actorIndex++;
	}


	set_status_text("Loaded actors from " + saveFileName);
}

void action_vehicle_chase() {
	log_to_file("action_vehicle_chase");
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (is_chase_player_engaged) {
		for (auto &actor : actors) {
			if (actor.isActorThisPed(playerPed) == false) {
				AI::TASK_PAUSE(actor.getActorPed(), 500);
			}
		}

		is_chase_player_engaged = false;
		nextWaitTicks = 500;
		set_status_text("Vehicle chase stopped");
		log_to_file("action_vehicle_chase stopped. AI::TASK_PAUSE sent to all actors");
		return;
	}


	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {

		Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);

		for (auto &actor : actors) {
			if (actor.isActorThisPed(playerPed) == false) {
				Ped actorPed = actor.getActorPed();
				ensure_ped_and_vehicle_is_not_deleted(actorPed);

				Vehicle actorVehicle = PED::GET_VEHICLE_PED_IS_USING(actorPed);

				//chase is based on type of vehicles for both player and actor
				if (PED::IS_PED_IN_ANY_HELI(playerPed) && PED::IS_PED_IN_ANY_HELI(actorPed)) {
					//both are in heli
					log_to_file("TASK_HELI_CHASE1 - Actor " + std::to_string(actorPed));
					//last three params are distance to playerVehicle
					AI::TASK_HELI_CHASE(actorPed, playerVehicle, 0.0, 0.0, -5.0);
				}
				else if (PED::IS_PED_IN_ANY_HELI(actorPed)) {
					//if only actor (not player) is in heli
					log_to_file("TASK_HELI_CHASE2 - Actor " + std::to_string(actorPed));
					AI::TASK_HELI_CHASE(actorPed, playerVehicle, 0.0, 0.0, -5.0);
				}
				else if (PED::IS_PED_IN_ANY_PLANE(playerPed) && PED::IS_PED_IN_ANY_PLANE(actorPed)) {
					//both are in a plane
					log_to_file("TASK_PLANE_CHASE1 - Actor " + std::to_string(actorPed));
					AI::TASK_PLANE_CHASE(actorPed, playerVehicle, 0.0, -5.0, -5.0);
				}
				else if (PED::IS_PED_IN_ANY_PLANE(actorPed)) {
					//if only actor (not player) is in plane
					log_to_file("TASK_PLANE_CHASE2 - Actor " + std::to_string(actorPed));
					AI::TASK_PLANE_CHASE(actorPed, playerVehicle, 0.0, 0.0, 5.0);
				}
				else {//standard vehicle chase
					log_to_file("TASK_VEHICLE_CHASE - Actor " + std::to_string(actorPed));
					AI::TASK_VEHICLE_CHASE(actorPed, playerPed);
				}
			}
		}
		set_status_text("Vehicle chase has started");
		is_chase_player_engaged = true;

		nextWaitTicks = 500;
	}
	else {
		log_to_file("action_vehicle_chase not started. Player missing vehicle");
		set_status_text("You need a vehicle in order to start a vehicle chase");
		nextWaitTicks = 200;
	}
}

void action_vehicle_escort() {
	log_to_file("action_vehicle_escort");
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	if (is_escort_player_engaged) {
		for (auto &actor : actors) {
			if (actor.isActorThisPed(playerPed) == false) {
				AI::TASK_PAUSE(actor.getActorPed(), 500);
			}
		}
		is_escort_player_engaged = false;
		nextWaitTicks = 500;
		log_to_file("action_vehicle_chase stopped. AI::TASK_PAUSE sent to all actors");
		set_status_text("Vehicle escort stopped");
		return;
	}

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		for (auto &actor : actors) {
			if (actor.isActorThisPed(playerPed) == false) {
				Ped actorPed = actor.getActorPed();
				ensure_ped_and_vehicle_is_not_deleted(actorPed);

				if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {

					Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorPed);

					//check if player is a passenger
					Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);

					if (pedDriver != playerPed) {
						//AI::TASK_VEHICLE_ESCORT(pedDriver, pedVehicle, playerVehicle, -1, VEHICLE::_GET_VEHICLE_MAX_SPEED(pedVehicle), 786469, 10.0, -1, 10.0);

						//works
						//AI::TASK_VEHICLE_ESCORT(pedDriver, pedVehicle, playerVehicle, -1, 13.0, 786603, 8.0, 20, 5.0);
						log_to_file("action_vehicle_escort:AI::TASK_VEHICLE_ESCORT Driver: " + std::to_string(pedDriver));
						AI::TASK_VEHICLE_ESCORT(pedDriver, pedVehicle, playerVehicle, -1, 45.0, actor.getDrivingMode().value, 8.0, 20, 5.0);

					}
				}

			}
		}

		is_escort_player_engaged = true;

		set_status_text("Vehicle escort has started");
		nextWaitTicks = 300;
	}
	else {
		log_to_file("action_vehicle_escort not started. Player missing vehicle");
		set_status_text("You need a vehicle in order to start a vehicle escort");
	}
}

void action_autopilot_for_player(bool suppressMessage) {
	log_to_file("action_autopilot_for_player");
	nextWaitTicks = 300;

	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	//update the waypoint if one is set currently
	if (actor.isNullActor()==false && UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		actor.setWaypoint(waypointCoord);
		actor.setHasWaypoint(true);
	}

	if (actor.isNullActor() == false && actor.hasWaypoint()) {
		if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actor.getActorPed());

			//check if player is a passenger
			Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
			log_to_file("action_autopilot_for_player " + std::to_string(pedDriver) +" vs " + std::to_string(actor.getActorPed()) + " vehicle " + std::to_string(pedVehicle));
			if (pedDriver == actor.getActorPed()) {
				bool isActive = move_to_waypoint(actor.getActorPed(), actor.getWaypoint(),true);
				
				if (isActive) {
					log_to_file("Autopilot engaged for player " + std::to_string(actor.getActorPed()));
					if (suppressMessage == false) {
						set_status_text("Autopilot engaged for player");
					}


					is_autopilot_engaged_for_player = true;
				}
				else {
					if (suppressMessage == false) {
						set_status_text("Set scene to active in order to enable autopilot");
					}
				}
			}
		}
		else {//if not in a vehicle, walk to the waypoint
			bool isActive = move_to_waypoint(actor.getActorPed(), actor.getWaypoint(), true);
			if (isActive) {
				if (suppressMessage == false) {
					set_status_text("Autopilot engaged for player");
				}
				is_autopilot_engaged_for_player = true;
			}
			else {
				if (suppressMessage == false) {
					set_status_text("Set scene to active in order to enable autopilot");
				}
			}
		}
	} else {
		if (suppressMessage == false) {
			set_status_text("Actor must be assigned a slot 1-9 and have a waypoint before autopilot can be started");
		}
	}
}


void action_set_same_waypoint_for_all_actors() {
	if (UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		log_to_file("action_set_same_waypoint_for_all_actors x:" + std::to_string(waypointCoord.x)+ " y:" + std::to_string(waypointCoord.y) + " z:" + std::to_string(waypointCoord.z));

		set_status_text("Waypoint set for all actors");

		//add waypoint to all actors in slots
		for (auto &actor : actors) {
			actor.setActiveInScene(sceneMode);
			actor.setWaypoint(waypointCoord);
			actor.setHasWaypoint(true);

			//act on the waypoint (will not do anything if SCENE_MODE_SETUP)

			//first if he's a driver
			move_to_waypoint(actor.getActorPed(), actor.getWaypoint(), true);
			//second if he's a passenger
			check_if_ped_is_passenger_and_has_waypoint(actor.getActorPed());
			WAIT(200);
		}


	}
	else {
		set_status_text("Set a waypoint in the map, before applying it to all actors");
	}
}

void action_teleport_to_start_locations() {
	set_status_text("Setting up the scene. Will take a few seconds");
	log_to_file("action_teleport_to_start_locations");

	bool haveDeadActors = false;

	std::map <Vehicle, std::tuple<DWORD, Vector3, float>> vehicleFirstRecorded;

	bool existActorWithStartLocation = false;

	for (auto &actor : actors) {
		if(actor.isNullActor()==false && actor.hasStartLocation()){
			existActorWithStartLocation = true;
			Ped entityToTeleport = actor.getActorPed();
			log_to_file("action_teleport_to_start_locations actor:"+std::to_string(entityToTeleport));
			float entityToTeleportHeading = actor.getStartLocationHeading();
			Vector3 entityToTeleportLocation = actor.getStartLocation();
			Ped actorPed = actor.getActorPed();

			PED::RESET_PED_VISIBLE_DAMAGE(actorPed);
			PED::CLEAR_PED_BLOOD_DAMAGE(actorPed);
			PED::SET_PED_SWEAT(actorPed, 0.0);

			//ressurect any dead actors
			if (ENTITY::IS_ENTITY_DEAD(actorPed)) {
				log_to_file("First revive");
				haveDeadActors = true;
				ENTITY::SET_ENTITY_HEALTH(actor.getActorPed(), ENTITY::GET_ENTITY_MAX_HEALTH(actorPed));
				Vector3 location = actor.getStartLocation();
				location.z = location.z + 1.8f;
				teleport_entity_to_location(entityToTeleport, location, true);

				//see http://gtaforums.com/topic/801452-death-recording-no-more-wastedbusted-screen-automatic-radio-off/
				PED::SET_PED_CAN_RAGDOLL(actorPed, true);
				AI::CLEAR_PED_TASKS_IMMEDIATELY(actorPed);
				teleport_entity_to_location(entityToTeleport, location, true);

			}


			if (actor.hasRecording()) {
				
				if (actor.hasStartLocationVehicle()) {
					log_to_file("Actor " + std::to_string(actor.getActorPed()) + " has recording with start location vehicle");
					entityToTeleport = actor.getStartLocationVehicle();
					Vehicle vehicleToTeleport = actor.getStartLocationVehicle();
					entityToTeleportLocation = actor.getStartLocationVehicleLocation();
					entityToTeleportHeading = actor.getStartLocationVehicleHeading();

					if (!PED::IS_PED_IN_VEHICLE(actorPed, entityToTeleport, false)) {
						PED::SET_PED_INTO_VEHICLE(actorPed, entityToTeleport, actor.getStartLocationVehicleSeat());
					}

					VEHICLE::SET_VEHICLE_ENGINE_ON(entityToTeleport, false, true,false);
					VEHICLE::SET_VEHICLE_UNDRIVEABLE(entityToTeleport, true);

					log_to_file("Before vehicleFirstRecorded");
					//update if it's the first time we see vehicle or if the start time is less
					if (vehicleFirstRecorded.find(vehicleToTeleport) == vehicleFirstRecorded.end())
					{
						vehicleFirstRecorded[vehicleToTeleport] = std::make_tuple(0, entityToTeleportLocation, entityToTeleportHeading);
					}
					log_to_file("After vehicleFirstRecorded");
				}
				else if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
					log_to_file("Actor " + std::to_string(actorPed) + " has recording without start location vehicle and is in a vehicle");
					AI::TASK_LEAVE_VEHICLE(actorPed, PED::GET_VEHICLE_PED_IS_USING(actorPed), 4160);
					WAIT(500);
				}
			}
			else {
				if (PED::IS_PED_IN_ANY_VEHICLE(entityToTeleport, 0)) {
					entityToTeleport = PED::GET_VEHICLE_PED_IS_USING(entityToTeleport);
					VEHICLE::SET_VEHICLE_ENGINE_ON(entityToTeleport, false, true,false);
					VEHICLE::SET_VEHICLE_UNDRIVEABLE(entityToTeleport, true);
				}
			}
			
			log_to_file("Before TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS");
			//try to prevent fleeing during teleport
			AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actorPed, true);
			PED::SET_PED_FLEE_ATTRIBUTES(actorPed, 0, 0);
			
			log_to_file("Before teleport_entity_to_location");
			//teleport and wait
			teleport_entity_to_location(entityToTeleport, entityToTeleportLocation, true);
			ENTITY::SET_ENTITY_HEADING(entityToTeleport, entityToTeleportHeading);
			log_to_file("Before wait");
			WAIT(300);
		}

	}
	if (!existActorWithStartLocation) {
		set_status_text("No actor with start location. Record or set scene to active first to get a start location");
		return;
	}

	log_to_file("Setting vehicle to undrivable");

	log_to_file("Checking if there are other used vehicles which should be teleported");

	for (auto &actor : actors) {
		if (actor.isNullActor() == false && actor.hasStartLocation()) {
			Ped actorPed = actor.getActorPed();
			if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
				Vehicle teleportedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorPed);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(teleportedVehicle, true);
				VEHICLE::SET_VEHICLE_ALARM(teleportedVehicle, true);
				VEHICLE::START_VEHICLE_ALARM(teleportedVehicle);
			}

			if (actor.hasRecording()) {
				log_to_file("About to do dynamic_pointer_cast");
				std::vector<std::shared_ptr<ActorRecordingItem>> actorRecording =  actor.getRecording();
				for (auto &recordingitem : actorRecording) {
					std::shared_ptr<ActorVehicleRecordingItem> vehicleRecording = std::dynamic_pointer_cast<ActorVehicleRecordingItem>(recordingitem);

					if (vehicleRecording != NULL) {
						Vehicle veh = vehicleRecording->getVehicle();
						//log_to_file("ActorVehicleRecordingItem with vehicle " + std::to_string(veh) + " heading " + std::to_string(vehicleRecording->getVehicleHeading()));

						//update if it's the first time we see vehicle or if the start time is less
						if (vehicleFirstRecorded.find(veh) == vehicleFirstRecorded.end() || std::get<0>(vehicleFirstRecorded[veh]) > vehicleRecording->getTicksAfterRecordStart())
						{
							vehicleFirstRecorded[veh] = std::make_tuple(vehicleRecording->getTicksAfterRecordStart(), vehicleRecording->getLocation(), vehicleRecording->getVehicleHeading());
						}

					}


				}


			}
		}
	}

	log_to_file("Teleporting all vehicles used in recordings");

	for (auto const &vehicleToBeTeleported : vehicleFirstRecorded) {
		
		log_to_file("Teleporting back to start location vehicle " + std::to_string(vehicleToBeTeleported.first));
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(vehicleToBeTeleported.first, true, true);
		//teleport and wait
		teleport_entity_to_location(vehicleToBeTeleported.first, std::get<1>(vehicleToBeTeleported.second), true);
		ENTITY::SET_ENTITY_HEADING(vehicleToBeTeleported.first, std::get<2>(vehicleToBeTeleported.second));

		VEHICLE::SET_VEHICLE_DOORS_SHUT(vehicleToBeTeleported.first, true);

		WAIT(300);
	}


	WAIT(1000);

	log_to_file("Setting vehicle to drivable");
	Ped orgPed = PLAYER::PLAYER_PED_ID();

	for (auto &actor : actors) {
		if (actor.isNullActor() == false && actor.hasStartLocation() && actor.isActorThisPed(orgPed)==false) {
			Ped actorPed = actor.getActorPed();
			possess_ped(actorPed);
			WAIT(250);
			if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
				log_to_file("Setting vehicle back to drivable for actor " + std::to_string(actorPed));
				Vehicle teleportedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorPed);
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(teleportedVehicle);
				VEHICLE::SET_VEHICLE_ALARM(teleportedVehicle, false);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(teleportedVehicle, false);
				VEHICLE::SET_VEHICLE_DOORS_SHUT(teleportedVehicle, true);
			}
		}
	}

	possess_ped(orgPed);
	if (PED::IS_PED_IN_ANY_VEHICLE(orgPed, 0)) {
		Vehicle teleportedVehicle = PED::GET_VEHICLE_PED_IS_USING(orgPed);
		VEHICLE::SET_VEHICLE_ALARM(teleportedVehicle, false);
		VEHICLE::SET_VEHICLE_UNDRIVEABLE(teleportedVehicle, false);
		VEHICLE::SET_VEHICLE_DOORS_SHUT(teleportedVehicle, true);
	}

	if (haveDeadActors) {
		WAIT(1000);
		for (auto &actor : actors) {
			Ped actorPed = actor.getActorPed();
			if (actor.isNullActor() == false && actor.hasStartLocation() && PED::IS_PED_FALLING(actorPed) ) {
				log_to_file(std::to_string(actorPed) + " is falling or dead after we had dead peds. Trying to teleport back to top");
				ENTITY::SET_ENTITY_HEALTH(actorPed, ENTITY::GET_ENTITY_MAX_HEALTH(actorPed));
				Vector3 location = actor.getStartLocation();
				location.z = location.z + 2.5f;
				teleport_entity_to_location(actorPed, location, true);
			}
			else if (actor.isNullActor() == false && actor.hasStartLocation() && (ENTITY::IS_ENTITY_DEAD(actorPed) || PED::IS_PED_DEAD_OR_DYING(actorPed, true))) {
				log_to_file(std::to_string(actorPed) + " is still dead. Real desparte attempts at reviving");

				ENTITY::SET_ENTITY_HEALTH(actorPed, ENTITY::GET_ENTITY_MAX_HEALTH(actorPed));
				Vector3 location = actor.getStartLocation();
				location.z = location.z + 2.2f;
				int ressurectTries = 0;
				while (ressurectTries < 2){
					PED::SET_PED_CAN_RAGDOLL(actorPed, true);
					AI::CLEAR_PED_TASKS_IMMEDIATELY(actorPed);
					teleport_entity_to_location(actorPed, location, true);
					ressurectTries++;
					WAIT(ressurectTries * 400);
				}
			}
		}
	}

	//update setting for all actors
	DWORD ticksStartAllActors = GetTickCount();
	for (auto & actor : actors) {
		if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
			ActorRecordingPlayback & otherActorRecordingPlayback = actor.getRecordingPlayback();
			otherActorRecordingPlayback.setHasTeleportedToStartLocation(ticksStartAllActors);

			if (actor.hasRecordingWithGunFire() && actor.isActorThisPed(PLAYER::PLAYER_PED_ID())) {
				set_status_text("Switch actor in order for gun fire recording to be playbacked correctly!");
			}
		}
	}
}

void action_timelapse_tick() {
	TIME::ADD_TO_CLOCK_TIME(0, timelapse_delta_minutes, timelapse_delta_seconds);
	//log_to_file("Clock after timelapse: " + std::to_string(TIME::GET_CLOCK_HOURS()) + ":" + std::to_string(TIME::GET_CLOCK_MINUTES()) + ":" + std::to_string(TIME::GET_CLOCK_SECONDS()));
}

void action_toggle_timelapse() {
	if (is_timlapse_active == false) {
		is_timlapse_active = true;
		timelapse_delta_seconds = GetPrivateProfileInt("timelapse", "timelapse_delta_seconds", 0, config_path);
		timelapse_delta_minutes = GetPrivateProfileInt("timelapse", "timelapse_delta_minutes", 2, config_path);

		set_status_text("Timelapse is active. Adding " + std::to_string(timelapse_delta_minutes) + " minutes and " +std::to_string(timelapse_delta_seconds) + " seconds to game clock every second. Adjustable in .ini file.");
	}
	else {
		is_timlapse_active = false;
		set_status_text("Timelapse is stopped");
	}
	nextWaitTicks = 100;
}

void action_toggle_blackout() {
	if (is_world_blackout == false) {
		GRAPHICS::_SET_BLACKOUT(true);
		is_world_blackout = true;
		set_status_text("Blackout of world started");
	}
	else {
		GRAPHICS::_SET_BLACKOUT(false);
		is_world_blackout = false;
		set_status_text("Blackout of world stopped");
	}
	nextWaitTicks = 100;
}

void action_next_animation_flag() {
	animationFlag = getNextAnimationFlag(animationFlag);
	if (animationFlag.name.compare("Controllable*") == 0) {
		set_status_text("Only the first animation in a sequence is used when Controllable is selected");
	}
}

void action_next_actor_prop() {
	currentActorProp = getNextActorProp(currentActorProp);
}

void action_next_weather() {
	index_weather++;
	if (index_weather > gtaWeatherTypes.size() - 1) {
		GAMEPLAY::CLEAR_WEATHER_TYPE_PERSIST();
		GAMEPLAY::SET_RANDOM_WEATHER_TYPE();
		index_weather = -1;
		set_status_text("Weather is now back to normal");
	}
	else {
		Weather weather = gtaWeatherTypes[index_weather];

		GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST(weather.id);
		//set_status_text("Weather is now: " + std::string(weather.id));
	}
}


void action_next_health() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor() == false) {
		Ped actorPed = actor.getActorPed();
		int currMaxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(actorPed);
		int newMaxHealth = currMaxHealth + 100;
		if (newMaxHealth > 500) {
			newMaxHealth = 100;
		}

		PED::SET_PED_MAX_HEALTH(actorPed, newMaxHealth);
		ENTITY::SET_ENTITY_HEALTH(actorPed, newMaxHealth);
	}
}


void action_toggle_vehicle_cosmetic() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	Ped actorPed = actor.getActorPed();
	if (actor.isNullActor() == false && PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {

		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorPed);
		if (actor.hasVehicleNoDamage()) {
			actor.setVehicleNoDamage(false);
			ENTITY::SET_ENTITY_INVINCIBLE(pedVehicle, false);
			ENTITY::SET_ENTITY_PROOFS(pedVehicle, 0, 0, 0, 0, 0, 0, 0, 0);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(pedVehicle, 1);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(pedVehicle, 1);
			VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(pedVehicle, 1);
		}
		else {
			actor.setVehicleNoDamage(true);
			ENTITY::SET_ENTITY_INVINCIBLE(pedVehicle, false);
			ENTITY::SET_ENTITY_PROOFS(pedVehicle, 1, 1, 1, 1, 1, 1, 1, 1);
			VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(pedVehicle, 0);
			VEHICLE::SET_VEHICLE_WHEELS_CAN_BREAK(pedVehicle, 0);
			VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(pedVehicle, 0);	
		}
	}
}
	


void action_toggle_wind() {
	if (is_wind_active ==false) {
		is_wind_active = true;
		GAMEPLAY::SET_WIND(1.0);
		GAMEPLAY::SET_WIND_SPEED(11.99f);
		GAMEPLAY::SET_WIND_DIRECTION(ENTITY::GET_ENTITY_HEADING(PLAYER::PLAYER_PED_ID()));
		set_status_text("Strong winds activated");
	}
	else {
		is_wind_active = false;
		GAMEPLAY::SET_WIND(0.0);
		GAMEPLAY::SET_WIND_SPEED(0.0);
		set_status_text("Winds back to normal");
	}
}

void action_next_walking_style() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	index_walking_style++;
	if (index_walking_style > gtaWalkingStyles.size() - 1) {
		PED::RESET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), 0.0);
		actor.setHasWalkingStyle(false);
		actor.setWalkingStyle( ClipSet());
		index_walking_style = -1;
		set_status_text("Walking style is now back to normal");
	}
	else {
		ClipSet walkingStyle = gtaWalkingStyles[index_walking_style];
		if (STREAMING::HAS_CLIP_SET_LOADED(walkingStyle.id)) {
			PED::SET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), walkingStyle.id, 1.0);
			actor.setHasWalkingStyle(true);
			actor.setWalkingStyle(walkingStyle);
		}
		else {
			log_to_file("Clipset has not loaded yet");
		}
		
		//set_status_text("Weather is now: " + std::string(weather.id));
	}
}

void action_next_walking_speed() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor() == false) {
		if (actor.getWalkingSpeed() == 2.0) {
			actor.setWalkingSpeed(1.0);
		}
		else {
			actor.setWalkingSpeed(2.0);
		}
		action_autopilot_for_player(true);
	}
}

void action_next_spot_light() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	log_to_file("action_next_spot_light for actor " + std::to_string(actor.getActorPed()));
	if (actor.isNullActor() == false) {
		if (actor.hasSpotLight()==false) {
			log_to_file("Turning on spot light for actor " + std::to_string(actor.getActorPed()));
			actor.setHasSpotLight(true);
			actor.setSpotLightType(SPOT_LIGHT_ACTOR_ABOVE);
			actor.setSpotLightColor(getDefaultSpotLightColor());
			log_to_file("actor.hasSpotLight() after " + std::to_string(actor.hasSpotLight()));
			for (auto &actor : actors) {
				log_to_file("vector actors.hasSpotLight() after " + std::to_string(actor.hasSpotLight()));
			}
			
		}
		else {
			if (actor.getSpotLightType() == last_spot_light_type) {
				actor.setHasSpotLight(false);
				actor.setSpotLightType(SPOT_LIGHT_NONE);
				log_to_file("Turning off spot light for actor " + std::to_string(actor.getActorPed()));
			}
			else {
				int existing_spot_light_type = actor.getSpotLightType();
				actor.setSpotLightType((SPOT_LIGHT_TYPE) (++existing_spot_light_type));
			}	
		}
	}
}

void action_next_driving_mode() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor() == false) {
		actor.setDrivingMode(getNextDrivingMode(actor.getDrivingMode()));
		action_autopilot_for_player(true);
	}
}

void action_edit_name() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());

	set_status_text("Enter new name for actor");
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "FMMC_KEY_TIP8", "", "", "", "", "", 6);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		WAIT(0);
	}


	if (GAMEPLAY::IS_STRING_NULL_OR_EMPTY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) {
		log_to_file("Got null keyboard value");
		return;
	}
	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	std::string strName = std::string(keyboardValue);
	log_to_file("Got keyboard value " + strName);

	actor.setName(strName);
}


void action_next_spot_light_color() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor() == false) {
		if (actor.hasSpotLight()) {
			actor.setSpotLightColor(getNextSpotLightColor(actor.getSpotLightColor()));
		}
	}
}

void action_next_relationshipgroup() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor() == false) {
		actor.setRelationshipGroup(getNextRelationshipGroup(actor.getRelationshipGroup()));
		if (sceneMode == SCENE_MODE_ACTIVE) {
			assign_actor_to_relationship_group(PLAYER::PLAYER_PED_ID(), actor.getRelationshipGroup());
		}
	}
}

void action_animation_lipmovement_start() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	STREAMING::REQUEST_ANIM_DICT("mp_facial");
	while (!STREAMING::HAS_ANIM_DICT_LOADED("mp_facial"))
	{
		WAIT(0);
	}

	//Animation flags are type "Facial"
	AI::TASK_PLAY_ANIM(actor.getActorPed(), "mp_facial", "mic_chatter", 8.0f, -8.0f, -1, ANIMATION_LOOP_FLAG1 | ANIMATION_ALLOW_MOVEMENT_FLAG6, 8.0f, 0, 0, 0);
}
void action_animation_lipmovement_stop() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	AI::STOP_ANIM_TASK(actor.getActorPed(), "mp_facial", "mic_chatter", -4.0);
}



void action_animation_single() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor()) {
		set_status_text("Switch to an actor before playing animation");
		return;
	}

	Ped actorPed = actor.getActorPed();
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "FMMC_KEY_TIP8", "", "", "", "", "", 6);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		WAIT(0);
	}

	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	std::string strAnimationIndex = std::string(keyboardValue);
	log_to_file("Got keyboard value " + strAnimationIndex);

	Animation animation = getAnimationForShortcutIndex(keyboardValue);
	if (animation.shortcutIndex != 0) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);

		DWORD ticksStart = GetTickCount();

		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() > ticksStart + 5000) {
				//duration will be 0 if it's not loaded
				log_to_file("Ticks overflow2");
				set_status_text("Could not load animation with code " + strAnimationIndex);
				return;
			}
		}




		int duration = 500;
		if (animationFlag.isFacialAnimation()) {
			if (animationPrevious.shortcutIndex != 0) {
				AI::STOP_ANIM_TASK(actorPed, animationPrevious.animLibrary, animationPrevious.animName, -4.0);
			}
			PED::PLAY_FACIAL_ANIM(actorPed, animation.animLibrary, animation.animName);
		}
		else {
			AI::TASK_PLAY_ANIM(actorPed, animation.animLibrary, animation.animName, 8.0f, -8.0f, -1, animationFlag.id, 8.0f, 0, 0, 0);
			animationPrevious = animation;
		}

	}
}


void action_animation_sequence_play(AnimationSequence animSequence) {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor()) {
		set_status_text("Switch to an actor before performing an animation");
		return;
	}
	Ped actorPed = actor.getActorPed();

	//load animation dicts
	for (auto &animation : animSequence.animationsInSequence) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);

		DWORD ticksStart = GetTickCount();

		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
			if (GetTickCount() > ticksStart + 5000) {
				//duration will be 0 if it's not loaded
				log_to_file("Ticks overflow2");
				set_status_text("Could not load animation with code " + std::string(animation.animLibrary));
				return;
			}
		}
	}
	//for (auto &animation : animSequence.animationsInSequence) {
		//ENTITY::PLAY_ENTITY_ANIM(actor.getActorPed(), animation.animLibrary,animation.animName,  1000.0, 0, 0, 0, 0, 0);
		//ENTITY::FORCE_ENTITY_AI_AND_ANIMATION_UPDATE(actor.getActorPed());
	//}
	
	//create task sequence
	currentTaskSequence += 1;
	TaskSequence task_seq = rand();
	AI::OPEN_SEQUENCE_TASK(&task_seq);

	//load animation dicts
	for (auto &animation : animSequence.animationsInSequence) {
		try {
			log_to_file("AI::TASK_PLAY_ANIM " + std::to_string(task_seq) + " - " + std::to_string(animation.shortcutIndex) + " " + std::string(animation.animName) + " " + std::string(animation.animLibrary) + " " + std::to_string(animation.duration) + " " + std::to_string(animationFlag.id));
			AI::TASK_PLAY_ANIM(0, animation.animLibrary, animation.animName, 8.0f, -8.0f, animation.duration, animationFlag.id, 8.0f, 0, 0, 0);
		}
		catch (const std::exception& e) {
			log_to_file("Exception " + std::string(e.what()));
		}
	}
	log_to_file("AI::CLOSE_SEQUENCE_TASK ");

	AI::CLOSE_SEQUENCE_TASK(task_seq);
	AI::TASK_PERFORM_SEQUENCE(actorPed, task_seq);
	AI::CLEAR_SEQUENCE_TASK(&task_seq);
	log_to_file("end of action_animation_sequence_play ");
}

void action_animation_sequence_add() {
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	Ped actorPed = actor.getActorPed();
	
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "INVALID_IN_ORDER_TO_DISPLAY_NOTHING", "", "", "", "", "", 256);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		DRAW_TEXT("View animations on Youtube http://bit.ly/GTAVAnims", 0.3, 0.30, 0.3, 0.3, 0, true, false, false, false, 255, 255, 255, 255);
		DRAW_TEXT("Add one or more animations to an animation sequence below", 0.3, 0.325, 0.3, 0.3, 0, true, false, false, false, 255, 255, 255, 255);
		DRAW_TEXT("Syntax: <00000-21822> <00000-21822>...", 0.3, 0.35, 0.3, 0.3, 0, true, false, false, false, 255, 255, 255, 255);
		WAIT(0);
	}

	if (GAMEPLAY::IS_STRING_NULL_OR_EMPTY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) {
		log_to_file("Got null keyboard value");
		return;
	}

	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	std::string strAnimationIndex = std::string(keyboardValue);
	log_to_file("Got keyboard value " + strAnimationIndex);

	char* token = strtok(keyboardValue, " ");
	std::vector<Animation> animations = {};
	int i = 0;
	int maxAnimations = 15;
	while (token != NULL)
	{
		log_to_file("Finding animation for token " + std::string(token));
		Animation animation = getAnimationForShortcutIndex(token);
		if (animation.shortcutIndex != 0 && animations.size() < maxAnimations) {
			log_to_file("Adding animation " + animation.toString());
			animations.push_back(animation);
		}
		
		token = strtok(NULL, " ");
		i++;
		if (i > 100) {
			return;
		}
	}

	if (animations.size() > 0) {
		//we currently ignore the key bindings, is based on index in vector instead
		 AnimationSequence animSequence{ animations };
		 AnimationTrigger animTrigger;
		 animTrigger.setAnimationSequence(animSequence);
		 if (animationSequences.size() >= 20) {

			 animationSequences[animationSequencesIndex] = animTrigger;
			 animationSequencesIndex++;
		 }
		 else {
			 animationSequences.push_back(animTrigger);
		 }

		 log_to_file("Animation sequences " + std::to_string(animationSequences.size()));

		 action_animation_sequence_play(animSequence);
	}
	else {
		set_status_text("No valid animations IDs found in input");
		if (getAllAnimations().size() <= 1) {
			set_status_text("Missing or empty SceneDirectorAnim.txt");
		}
	}
}

void action_animation_sync_execute(Vector3 sceneLocation, std::vector<Actor>  syncActors, std::vector<Animation>  syncAnimations) {
	log_to_file("action_animation_sync_execute");
	if (syncActors[0].isNullActor() || syncActors[1].isNullActor()) {
		set_status_text("Must have at least two actors");
		return;
	}

	//load animations
	for (auto &synAnimation : syncAnimations) {
		STREAMING::REQUEST_ANIM_DICT(synAnimation.animLibrary);
		while (!STREAMING::HAS_ANIM_DICT_LOADED(synAnimation.animLibrary))
		{
			WAIT(0);
		}
	}

	int scene = PED::CREATE_SYNCHRONIZED_SCENE(sceneLocation.x, sceneLocation.y, sceneLocation.z, 0.0, 0.0, 180.0, 2);
	PED::SET_SYNCHRONIZED_SCENE_LOOPED(scene, false);
	int actorIndex = 0;
	for (auto &synAnimation : syncAnimations) {
		if (syncActors[actorIndex].isNullActor()) {
			set_status_text("Cannot play synced animation #" + std::to_string(actorIndex + 1) + "  since Actor " +std::to_string(actorIndex + 1) + " does not exist");
		}
		else {
			AI::TASK_SYNCHRONIZED_SCENE(syncActors[actorIndex].getActorPed(), scene, synAnimation.animLibrary, synAnimation.animName, 1000.0, -4.0, 64, 0, 0x447a0000, 0);
		}
		actorIndex++;
	}


	PED::SET_SYNCHRONIZED_SCENE_PHASE(scene, 0.0);
}

void action_animation_sync_setup() {
	log_to_file("action_animation_sync_setup");
	if (actors[0].isNullActor() || actors[1].isNullActor()) {
		set_status_text("Must have at least two actors");
		return;
	}

	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "INVALID_IN_ORDER_TO_DISPLAY_NOTHING", "", "", "", "", "", 256);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		DRAW_TEXT("Add two or more animations below that will be played with existing Actors", 0.3, 0.325, 0.3, 0.3, 0, true, false, false, false, 255, 255, 255, 255);
		DRAW_TEXT("Syntax: <00000-21822> <00000-21822>...", 0.3, 0.35, 0.3, 0.3, 0, true, false, false, false, 255, 255, 255, 255);
		WAIT(0);
	}

	if (GAMEPLAY::IS_STRING_NULL_OR_EMPTY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) {
		log_to_file("Got null keyboard value");
		return;
	}

	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	std::string strAnimationIndex = std::string(keyboardValue);
	log_to_file("Got keyboard value " + strAnimationIndex);

	char* token = strtok(keyboardValue, " ");
	std::vector<Animation> syncAnimations = {};
	int i = 0;
	int maxAnimations = 15;
	while (token != NULL)
	{
		log_to_file("Finding animation for token " + std::string(token));
		Animation animation = getAnimationForShortcutIndex(token);
		if (animation.shortcutIndex != 0 && syncAnimations.size() < maxAnimations) {
			log_to_file("Adding animation " + animation.toString());
			syncAnimations.push_back(animation);
		}

		token = strtok(NULL, " ");
		i++;
		if (i > 100) {
			return;
		}
	}

	if (syncAnimations.size() == 0) {
		set_status_text("No animations could be found");
		return;
	}
	else {
		Vector3 startLocation = ENTITY::GET_ENTITY_COORDS(actors[0].getActorPed(), true);
		action_animation_sync_execute(startLocation,actors, syncAnimations);
	}

}


void action_animations_preview(){
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor()) {
		set_status_text("Switch to an actor before starting preview");
		return;
	}
	Ped actorPed = actor.getActorPed();

	boolean customCamera = true; 

	animationFilterStr = std::string();
	hasAnimationFilter = false;
	doAnimationLoop = false;

	/*
	set_status_text("Enter animation code to begin preview");
	set_status_text("Rotate character before starting preview for different angle");
	set_status_text("Also available on Youtube through http://bit.ly/GTAVAnims");
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "FMMC_KEY_TIP8", "", "", "", "", "", 6);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		WAIT(0);
	}


	if (GAMEPLAY::IS_STRING_NULL_OR_EMPTY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) {
		log_to_file("Got null keyboard value");
		return;
	}
	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	std::string strAnimationIndex = std::string(keyboardValue);
	log_to_file("Got keyboard value " + strAnimationIndex);*/

	Animation animation = getAnimationForShortcutIndex("1");


	Vector3 startLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);
	float startHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
	log_to_file("Start heading is " + std::to_string(startHeading));
	Vector3 camOffset;
	camOffset.x = (float) sin((startHeading *PI / 180.0f))*3.0f;
	camOffset.y = (float) cos((startHeading *PI / 180.0f))*3.0f;



	if (startLocation.x < 0) {
		camOffset.x = -camOffset.x;
	}
	if (startLocation.y < 0) {
		camOffset.y = -camOffset.y;
	}


	camOffset.z = 0.4;

	log_to_file("Camera offset (" + std::to_string(camOffset.x) + ", " + std::to_string(camOffset.y) + ", " + std::to_string(camOffset.z) + ")");
	Vector3 camLocation = ENTITY::GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(actorPed, camOffset.x, camOffset.y, camOffset.z);
	Any cameraHandle;
	if (customCamera) {
		cameraHandle = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", camLocation.x, camLocation.y, camLocation.z, 0.0, 0.0, 0.0, 40.0, 1, 2);
		CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
		CAM::POINT_CAM_AT_ENTITY(cameraHandle, actorPed, 0.0f, 0.0f, 0.0f, true);
		CAM::RENDER_SCRIPT_CAMS(true, 0, 3000, 1, 0);
	}

	std::vector<Animation> animations = getAllAnimations();
	log_to_file("Have " + std::to_string(animations.size()) + " animations");

	if (animations.size() <= 1) {
		set_status_text("Missing or empty SceneDirectorAnim.txt");
		return;
	}


	float currentCamHeading = startHeading;
	/*Code for calculating length of animations 
	log_to_file("--------- Start anim length determination  -------");
	int animNewCounter = animation.shortcutIndex;
	for (int i = animation.shortcutIndex; i < animations.size(); i++) {
		Animation animation = animations[i];

		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
		DWORD ticksStart = GetTickCount();
		bool hasLoaded = true;
		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{

			WAIT(0);
			if (GetTickCount() > ticksStart + 1000) {
				log_to_file("Failed STREAMING::HAS_ANIM_DICT_LOADED for " + std::string(animation.animLibrary));
				hasLoaded = false;
				break;
			}

		}

		if (hasLoaded) {
			int duration = 500;
			AI::TASK_PLAY_ANIM(actorPed, animation.animLibrary, animation.animName, 8.0f, -8.0f, duration, true, 8.0f, 0, 0, 0);
			ticksStart = GetTickCount();
			while (!ENTITY::IS_ENTITY_PLAYING_ANIM(actorPed, animation.animLibrary, animation.animName, 3)) {
				WAIT(0);
				if (GetTickCount() > ticksStart + 1000) {
					//duration will be 0 if it's not loaded
					//log_to_file("Ticks overflow2");
					break;
				}
			}

			int animDuration = (int)ENTITY::GET_ENTITY_ANIM_TOTAL_TIME((Entity)actorPed, animation.animLibrary, animation.animName);
			if (animDuration != 0) {
				log_to_file(std::to_string(animNewCounter) + " " + std::string(animation.animLibrary) + " " + std::string(animation.animName) + " " + std::to_string(animDuration));
				animNewCounter++;
			}
		}
		//animNewCounter++;
	}
	log_to_file("--------- End anim length determination  -------");
	*/




	for (int i = animation.shortcutIndex; i < animations.size();i++) {
		Animation animation = animations[i];
		std::string strAnimation = animation.strShortcutIndex + " " + std::string(animation.animLibrary) + " " + std::string(animation.animName) + " " + std::to_string(animation.duration);

		if (hasAnimationFilter && !animation.matchesFilter(animationFilterStr)) {
			continue;
		}

		if (animation.shortcutIndex != 0 && animation.duration != 0) {
			STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
			DWORD ticksStart = GetTickCount();
			bool hasLoaded = true;
			while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
			{

				WAIT(0);
				draw_instructional_buttons_animation_preview();
				DRAW_TEXT(strdup(strAnimation.c_str()), 0.0, 0.0, 0.5, 0.5, 0, true, false, false, false, 255, 255, 255, 200);
				if (GetTickCount() > ticksStart + 10000) {
					log_to_file("Failed STREAMING::HAS_ANIM_DICT_LOADED for "+ std::string(animation.animLibrary));
					hasLoaded = false;
					break;
				}
				
			}

			if (hasLoaded) {
				int duration = 500;
				
				AI::TASK_PLAY_ANIM(actorPed, animation.animLibrary, animation.animName, 8.0f, -8.0f, animation.duration, true, 8.0f, 0, 0, 0);

				ticksStart = GetTickCount();
				while (!ENTITY::IS_ENTITY_PLAYING_ANIM(actorPed, animation.animLibrary, animation.animName, 3)) {
					WAIT(0);
					draw_instructional_buttons_animation_preview();
					DRAW_TEXT(strdup(strAnimation.c_str()), 0.0, 0.0, 0.5, 0.5, 0, true, false, false, false, 255, 255, 255, 200);

					CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
					if (IsKeyDown(VK_MENU) && IsKeyDown(0x43)) {//stop preview
						//reset cam
						CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);
						return;
					}

					if (GetTickCount() > ticksStart + 1000) {
						//duration will be 0 if it's not loaded
						//log_to_file("Ticks overflow2");
						break;
					}
				}
				log_to_file(animation.strShortcutIndex + " " + std::string(animation.animLibrary) + " " + std::string(animation.animName));
				ticksStart = GetTickCount();
				while (ENTITY::IS_ENTITY_PLAYING_ANIM(actorPed, animation.animLibrary, animation.animName, 3)) {
					WAIT(0);
					draw_instructional_buttons_animation_preview();
					DRAW_TEXT(strdup(strAnimation.c_str()), 0.0, 0.0, 0.5, 0.5, 0, true, false, false, false, 255, 255, 255, 255);

					CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
					if (IsKeyDown(0x43)) {//stop preview
						CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);
						return ;
					}
					else if (IsKeyDown(0x4E)) {//next animation
						if (hasAnimationFilter) {
							for (int j = i+1; j < animations.size(); j++) {
								if (animations[j].matchesFilter(animationFilterStr)) {
									i = j;
									WAIT(100);
									break;
								}
							}
						}
						else {
							if (doAnimationLoop) {
								i = i + 1;
							}
							WAIT(50);
						}
						break;
					}
					else if (IsKeyDown(0x42)) {//previous animation
						if (hasAnimationFilter) {
							for (int j = i - 2; j >= 0; j--) {
								if (animations[j].matchesFilter(animationFilterStr)) {
									i=j;
									WAIT(50);
									break;
								}
							}
						}
						else {
							if (doAnimationLoop) {
								i = i -1;
							}
							else {
								i = i - 2;
							}
							
							WAIT(50);
						}
						
						break;
					}
					else if (IsKeyDown(0x4C)) {//looping
						
						if (doAnimationLoop) {
							set_status_text("Stopped looping");
							doAnimationLoop = false;
						}
						else {
							doAnimationLoop = true;
							set_status_text("Now looping current animation");
						}
						
						WAIT(100);
					}
					else if (IsKeyDown(VK_LEFT)) {//previous animation
						currentCamHeading += 10.0;
						if (currentCamHeading >= 359.9) {
							currentCamHeading = 0.0;
						}
							
						camOffset.x = (float)sin((currentCamHeading *PI / 180.0f))*3.0f;
						camOffset.y = (float)cos((currentCamHeading *PI / 180.0f))*3.0f;
						//camLocation = ENTITY::GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(actorPed, camOffset.x, camOffset.y, camOffset.z);
						if (customCamera) {
							CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
							WAIT(100);
						}
					}
					else if (IsKeyDown(VK_RIGHT)) {//previous animation
						currentCamHeading -= 10.0;
						if (currentCamHeading < 0.0) {
							currentCamHeading += 360.0;
						}

						camOffset.x = (float)sin((currentCamHeading *PI / 180.0f))*3.0f;
						camOffset.y = (float)cos((currentCamHeading *PI / 180.0f))*3.0f;
						//camLocation = ENTITY::GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(actorPed, camOffset.x, camOffset.y, camOffset.z);
						if (customCamera) {
							CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
							WAIT(100);
						}


					}
					else if (IsKeyDown(0x46)) {//F key
						set_status_text("Enter text to filter animations on. Operators AND OR NOT can be used");
						animationFilterStr = actionInputString(50);
						if (!animationFilterStr.empty()) {
							int nrMatches = 0;
							for (int j = 0; j < animations.size(); j++) {
								if (animations[j].matchesFilter(animationFilterStr)) {
									nrMatches++;
								}
							}
							if (nrMatches == 0) {
								set_status_text("Found no animations matching filter " + animationFilterStr);
								hasAnimationFilter = false;
							}
							else {
								set_status_text("Found " +std::to_string(nrMatches) + " animations matching filter ");
								i = 0;
								hasAnimationFilter = true;
								doAnimationLoop = true;
								break;
							}
						}
						else {
							hasAnimationFilter = false;
							break;
						}
					}
					else if (IsKeyDown(0x47)) {//G - key

						set_status_text("Enter animation code to begin preview");
						GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "FMMC_KEY_TIP8", "", "", "", "", "", 6);

						while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
							WAIT(0);
						}


						if (GAMEPLAY::IS_STRING_NULL_OR_EMPTY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) {
							log_to_file("Got null keyboard value");
						}
						else {
							char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
							std::string strAnimationIndex = std::string(keyboardValue);
							log_to_file("Got keyboard value " + strAnimationIndex);

							Animation animation = getAnimationForShortcutIndex(keyboardValue);
							i = animation.shortcutIndex - 1;
							break;
						}
					}
					

					
					if (GetTickCount() > ticksStart + 60000) {
						//duration will be 0 if it's not loaded
						//log_to_file("Ticks overflow2");
						break;
					}

				} 



				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(actorPed, startLocation.x, startLocation.y, startLocation.z, 0, 0, 1);
				ENTITY::SET_ENTITY_HEADING(actorPed, startHeading);
			}
			else {
				log_to_file(animation.strShortcutIndex + " " + std::string(animation.animLibrary) + " " + std::string(animation.animName) + " " + std::to_string(0));
			}
			if (doAnimationLoop) {
				i = i - 1;
			}
		}
	}

	//reset cam
	CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);

	/*
	for (auto animation : animations) {
		if (animation.shortcutIndex != 0) {
			STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);
			DWORD ticksStart = GetTickCount();
			bool hasLoaded = true;
			while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
			{
				WAIT(0);
				if (GetTickCount() > ticksStart + 5000) {
					//log_to_file("Ticks overflow");
					hasLoaded = false;
					break;
				}
			}

			if (hasLoaded) {
				int duration = 500;
				AI::TASK_PLAY_ANIM(actorPed, animation.animLibrary, animation.animName, 8.0f, -8.0f, duration, true, 8.0f, 0, 0, 0);
				ticksStart = GetTickCount();
				while (!ENTITY::IS_ENTITY_PLAYING_ANIM(actorPed, animation.animLibrary, animation.animName, 3)) {
					WAIT(0);
					if (GetTickCount() > ticksStart + 5000) {
						//duration will be 0 if it's not loaded
						//log_to_file("Ticks overflow2");
						break;
					}
				}
				duration = (int)ENTITY::GET_ENTITY_ANIM_TOTAL_TIME((Entity)actorPed, animation.animLibrary, animation.animName);
				log_to_file(animation.strShortcutIndex + " " + std::string(animation.animLibrary) + " " + std::string(animation.animName) + " " + std::to_string(duration));
			}
			else {
				log_to_file(animation.strShortcutIndex + " " + std::string(animation.animLibrary) + " " + std::string(animation.animName) + " " + std::to_string(0));
			}
		}
	}*/
	/*
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "FMMC_KEY_TIP8", "", "", "", "", "", 6);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		WAIT(0);
	}

	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	std::string strAnimationIndex = std::string(keyboardValue);
	log_to_file("Got keyboard value " + strAnimationIndex);

	Animation animation = getAnimationForShortcutIndex(keyboardValue);
	if (animation.shortcutIndex != 0) {
		STREAMING::REQUEST_ANIM_DICT(animation.animLibrary);

		//const System::DateTime endtime = System::DateTime::Now + System::TimeSpan(0, 0, 0, 0, 1000);

		while (!STREAMING::HAS_ANIM_DICT_LOADED(animation.animLibrary))
		{
			WAIT(0);
		}

		int duration = 500;
		AI::TASK_PLAY_ANIM(actorPed, animation.animLibrary, animation.animName, 8.0f, -8.0f, duration, true, 8.0f, 0, 0, 0);
		while (!ENTITY::IS_ENTITY_PLAYING_ANIM(actorPed, animation.animLibrary, animation.animName, 3)) {
			WAIT(0);
		}
		duration = ENTITY::GET_ENTITY_ANIM_TOTAL_TIME((Entity)actorPed, animation.animLibrary, animation.animName);
		log_to_file("Animation" + std::string(animation.animLibrary) + "  duration  " + std::to_string(duration));
	}
	*/

}
void action_animation_sync_play(SyncedAnimation syncedAnimation, bool clearObjectReferences) {
	if (clearObjectReferences) {
		syncedAnimation.clearObjectReferences();
	}
	syncedAnimation.executeSyncedAnimation(false,getActorPointers(), true, Vector3(), false, true, 0.0f);
	DWORD ticksStart = GetTickCount();

	while (!syncedAnimation.isCompleted()) {
		WAIT(0);
		draw_spot_lights();

		if (GetTickCount() > ticksStart + 120000) {
			break;
		}

	}
	//cleanup objects and teleport back to start
	syncedAnimation.cleanupAfterExecution(true, true);
}

void action_animation_sync_add(SyncedAnimation syncedAnimation) {
	//we currently ignore the key bindings, is based on index in vector instead
	AnimationTrigger animTrigger;
	animTrigger.setSyncedAnimation(syncedAnimation);
	if (animationSequences.size() >= 20) {
		animationSequences[animationSequencesIndex] = animTrigger;
		animationSequencesIndex++;
	}
	else {
		animationSequences.push_back(animTrigger);
	}

	log_to_file("Animation sequences " + std::to_string(animationSequences.size()));
}

void action_animation_sync_preview() {
	log_to_file("action_animation_sync_execute");
	if (actors[0].isNullActor()) {
		set_status_text("Must have at least one actor before starting synchronized animation preview");
		return;
	}
	menu_active_index = 0;
	int menu_index_for_active_category = 0;

	Ped actorPed = actors[0].getActorPed();

	boolean customCamera = true;

	animationFilterStr = std::string();
	hasAnimationFilter = false;
	doAnimationLoop = true;

	Vector3 startLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);
	float startHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
	log_to_file("Start heading is " + std::to_string(startHeading));
	Vector3 camOffset;
	camOffset.x = (float)sin((startHeading *PI / 180.0f))*6.0f;
	camOffset.y = (float)cos((startHeading *PI / 180.0f))*6.0f;

	if (startLocation.x < 0) {
		camOffset.x = -camOffset.x;
	}
	if (startLocation.y < 0) {
		camOffset.y = -camOffset.y;
	}

	camOffset.z = 0.4;

	log_to_file("Camera offset (" + std::to_string(camOffset.x) + ", " + std::to_string(camOffset.y) + ", " + std::to_string(camOffset.z) + ")");
	Vector3 camLocation = ENTITY::GET_OFFSET_FROM_ENTITY_GIVEN_WORLD_COORDS(actorPed, camOffset.x, camOffset.y, camOffset.z);
	Any cameraHandle;
	if (customCamera) {
		cameraHandle = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", camLocation.x, camLocation.y, camLocation.z, 0.0, 0.0, 0.0, 40.0, 1, 2);
		CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
		CAM::POINT_CAM_AT_ENTITY(cameraHandle, actorPed, 0.0f, 0.0f, 0.0f, true);
		CAM::RENDER_SCRIPT_CAMS(true, 0, 3000, 1, 0);
	}

	std::vector<SyncedAnimation> gtaSyncedAnimations = getAllSyncedAnimations();




	set_status_text(std::to_string(gtaSyncedAnimations.size()) + " synchronized animations (with help from MuzTube)");
	log_to_file("Have " + std::to_string(gtaSyncedAnimations.size()) + " synchronized animations");

	float currentCamHeading = startHeading;

	bool continuePreviewMode = true;
	SyncedAnimation currentSyncedAnimation;
	DWORD ticksLast = GetTickCount();
	DWORD nextWaitTicksSyncAnim = 0;

	
	while (continuePreviewMode) {
		//check for input
		if (nextWaitTicksSyncAnim == 0 || GetTickCount() - ticksLast >= nextWaitTicksSyncAnim) {
			nextWaitTicksSyncAnim = 0;
			ticksLast = GetTickCount();

			if (menu_up_key_pressed()) {
				menu_alt_action_up();
				nextWaitTicksSyncAnim = 100;
			}
			else if (menu_down_key_pressed()) {
				menu_alt_action_down();
				nextWaitTicksSyncAnim = 100;
			}
			else if (menu_back_key_pressed()) {
				currentSyncedAnimCategory.clear();
				menu_active_index = menu_index_for_active_category;
				nextWaitTicksSyncAnim = 100;
			}
			else if (menu_select_key_pressed()) {//handle menu select
				if (menu_active_action >= MENU_ITEM_SYNCEDPREVIEW_CATEGORY1 && menu_active_action <= MENU_ITEM_SYNCEDPREVIEW_CATEGORY15) {
					int categoryIndex = menu_active_action -1 - MENU_ITEM_SYNCEDPREVIEW_CATEGORY1;

					currentSyncedAnimCategory = modSyncedAnimCategories[categoryIndex].first;
					log_to_file(currentSyncedAnimCategory);
					currentSyncedAnimations = getSyncedAnimations(currentSyncedAnimCategory);
					menu_index_for_active_category = menu_active_index;
					menu_active_index = 0;

				}
				else if (menu_active_action == MENU_ITEM_SYNCEDPREVIEW_SEARCH) {
					set_status_text("Enter text to filter synchronized animations on. Operators AND OR NOT can be used");
					animationFilterStr = actionInputString(100);
					if (!animationFilterStr.empty()) {
						int nrMatches = 0;
						currentSyncedAnimations = std::vector<SyncedAnimation>();
						for (int i = 0; i < gtaSyncedAnimations.size(); i++) {
							if (gtaSyncedAnimations[i].matchesFilter(animationFilterStr)) {
								currentSyncedAnimations.push_back(gtaSyncedAnimations[i]);
							}
						}
						if (nrMatches == 0) {
							set_status_text("Found no animations matching filter " + animationFilterStr);
							currentSyncedAnimCategory = "Search";
						}
						else {
							set_status_text("Found " + std::to_string(nrMatches) + " animations matching filter ");
							currentSyncedAnimCategory.empty();
						}
					}
				}
				else if (menu_active_action == MENU_ITEM_SYNCEDPREVIEW_BACK) {
					menu_active_index = menu_index_for_active_category;
					currentSyncedAnimCategory.clear();
				}
				else if (menu_active_action >= MENU_ITEM_SYNCEDPREVIEW_ANIMINDEX_START && menu_active_action <= MENU_ITEM_SYNCEDPREVIEW_ANIMINDEX_END) {
					int syncedAnimIndex = menu_active_action - MENU_ITEM_SYNCEDPREVIEW_ANIMINDEX_START;

					//cleanup old synced animation
					if (!currentSyncedAnimation.isNull()) {
						currentSyncedAnimation.cleanupAfterExecution(true, true);
					}

					currentSyncedAnimation = currentSyncedAnimations[syncedAnimIndex];
					currentSyncedAnimation.executeSyncedAnimation(false,getActorPointers(), true, Vector3(), true, true, 0.0f);
					log_to_file("Selected synced animation " + currentSyncedAnimation.getTitle());
				}
				nextWaitTicksSyncAnim = 200;

			}

			//check for instructional key input
			if (IsKeyDown(0x43)) {//stop preview
				if (!currentSyncedAnimation.isNull()) {
					currentSyncedAnimation.cleanupAfterExecution(true, true);
				}
				CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);
				return;
			}
			else if (IsKeyDown(VK_LEFT)) {//rotate camera to the left
				currentCamHeading += 10.0;
				if (currentCamHeading >= 359.9) {
					currentCamHeading = 0.0;
				}

				camOffset.x = (float)sin((currentCamHeading *PI / 180.0f))*6.0f;
				camOffset.y = (float)cos((currentCamHeading *PI / 180.0f))*6.0f;
				if (customCamera) {
					CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
					nextWaitTicksSyncAnim = 100;
				}
			}
			else if (IsKeyDown(VK_RIGHT)) {//rotate camera to the right
				currentCamHeading -= 10.0;
				if (currentCamHeading < 0.0) {
					currentCamHeading += 360.0;
				}

				camOffset.x = (float)sin((currentCamHeading *PI / 180.0f))*6.0f;
				camOffset.y = (float)cos((currentCamHeading *PI / 180.0f))*6.0f;
				if (customCamera) {
					CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
					nextWaitTicksSyncAnim = 100;
				}
			}
			else if (IsKeyDown(VK_ADD)) {//+
				log_to_file("UP");
				if (!currentSyncedAnimation.isNull()) {
					currentSyncedAnimation.setDeltaZ(currentSyncedAnimation.getDeltaZ() + 0.1);
					currentSyncedAnimation.cleanupAfterExecution(true, true);
					currentSyncedAnimation.executeSyncedAnimation(false,getActorPointers(), true, Vector3(), doAnimationLoop, true, 0.0f);
				}

				nextWaitTicksSyncAnim = 150;
			}
			else if (IsKeyDown(VK_SUBTRACT)) {//-
				log_to_file("Down");
				if (!currentSyncedAnimation.isNull()) {
					currentSyncedAnimation.setDeltaZ(currentSyncedAnimation.getDeltaZ() - 0.1);
					currentSyncedAnimation.cleanupAfterExecution(true, true);
					currentSyncedAnimation.executeSyncedAnimation(false,getActorPointers(), true, Vector3(), doAnimationLoop, true, 0.0f);
				}
				nextWaitTicksSyncAnim = 150;
			}
			else if (IsKeyDown(0x41)) {//A
				if (!currentSyncedAnimation.isNull()) {
					log_to_file("Adding " + currentSyncedAnimation.toString());
					action_animation_sync_add(currentSyncedAnimation);
					set_status_text("Added synchronized animation to shortcut");
				}
				nextWaitTicksSyncAnim = 150;
			}

			
		}

		//draw menus and other elements every tick
		draw_instructional_buttons_animation_sync_preview();
		draw_spot_lights();
		draw_menu_synced_anim_preview();

		//disable all the keys!
		CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);

		//draw name of current synced animation
		if (!currentSyncedAnimation.isNull()) {
			DRAW_TEXT(strdup(currentSyncedAnimation.toString().c_str()), 0.0, 0.0, 0.5, 0.5, 0, true, false, false, false, 255, 255, 255, 255);
		}
		WAIT(0);
	}

/*
	for (int i = 0; i < syncedAnimations.size(); i++) {
		SyncedAnimation syncedAnimation = syncedAnimations[i];

		if (hasAnimationFilter && !syncedAnimation.matchesFilter(animationFilterStr)) {
			continue;
		}


		DRAW_TEXT(strdup(syncedAnimation.toString().c_str()), 0.0, 0.0, 0.5, 0.5, 0, true, false, false, false, 255, 255, 255, 200);

		//std::vector<GTAObject>(),
		syncedAnimation.executeSyncedAnimation(getActorPointers(),  true, Vector3(), doAnimationLoop,true,0.0f);
		DWORD mainTickLast = GetTickCount();

		while (!syncedAnimation.isCompleted()) {
			DWORD ticksNow= GetTickCount();
			WAIT(0);
			draw_instructional_buttons_animation_sync_preview();
			draw_spot_lights();
			DRAW_TEXT(strdup(syncedAnimation.toString().c_str()), 0.0, 0.0, 0.5, 0.5, 0, true, false, false, false, 255, 255, 255, 255);

			if (nextWaitTicks == 0 || GetTickCount() - mainTickLast >= nextWaitTicks) {
			//if (ticksNow >= ticksLastMenu + 100) {
				if (menu_up_key_pressed()) {
					menu_alt_action_up();
				}
				else if (menu_down_key_pressed()) {
					menu_alt_action_down();
				}
				else if (menu_left_key_pressed()) {
					menu_action_left();
				}
				else if (menu_right_key_pressed()) {
					menu_action_right();
				}
				else if (menu_select_key_pressed()) {
					menu_action_select();
				}
				else if (menu_delete_key_pressed()) {
					menu_action_delete();
				}
			}

			draw_menu_synced_anim_preview();


			CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
			if (IsKeyDown(0x43)) {//stop preview
				syncedAnimation.cleanupAfterExecution(true, true);
				CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);
				return;
			}
			else if (IsKeyDown(0x4E)) {//next animation
				if (hasAnimationFilter) {
					for (int j = i + 1; j < syncedAnimations.size(); j++) {
						if (syncedAnimations[j].matchesFilter(animationFilterStr)) {
							i = j;
							WAIT(150);
							break;
						}
					}
				}
				else {
					WAIT(150);
				}
				break;
			}
			else if (IsKeyDown(0x42)) {//previous animation
				if (hasAnimationFilter) {
					for (int j = i - 2; j >= 0; j--) {
						if (syncedAnimations[j].matchesFilter(animationFilterStr)) {
							i = j;
							WAIT(150);
							break;
						}
					}
				}
				else {
					i = i - 2;
					WAIT(150);
				}

				break;
			}
			else if (IsKeyDown(0x4C)) {//previous animation

				if (doAnimationLoop) {
					doAnimationLoop = false;
					syncedAnimation.setLooping(doAnimationLoop);
					set_status_text("Stopped looping");

				}
				else {
					doAnimationLoop = true;
					syncedAnimation.setLooping(doAnimationLoop);
					set_status_text("Now looping current animation");
				}

				WAIT(150);
			}
			else if (IsKeyDown(VK_LEFT)) {//previous animation
				currentCamHeading += 10.0;
				if (currentCamHeading >= 359.9) {
					currentCamHeading = 0.0;
				}

				camOffset.x = (float)sin((currentCamHeading *PI / 180.0f))*6.0f;
				camOffset.y = (float)cos((currentCamHeading *PI / 180.0f))*6.0f;
				if (customCamera) {
					CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
					WAIT(100);
				}
			}
			else if (IsKeyDown(VK_RIGHT)) {//previous animation
				currentCamHeading -= 10.0;
				if (currentCamHeading < 0.0) {
					currentCamHeading += 360.0;
				}

				camOffset.x = (float)sin((currentCamHeading *PI / 180.0f))*6.0f;
				camOffset.y = (float)cos((currentCamHeading *PI / 180.0f))*6.0f;
				if (customCamera) {
					CAM::ATTACH_CAM_TO_ENTITY(cameraHandle, actorPed, camOffset.x, camOffset.y, camOffset.z, true);
					WAIT(100);
				}


			}
			else if (IsKeyDown(0x46)) {//F key
				set_status_text("Enter text to filter animations on. Operators AND OR NOT can be used");
				animationFilterStr = actionInputString(50);
				if (!animationFilterStr.empty()) {
					int nrMatches = 0;
					for (int j = 0; j < syncedAnimations.size(); j++) {
						if (syncedAnimations[j].matchesFilter(animationFilterStr)) {
							nrMatches++;
						}
					}
					if (nrMatches == 0) {
						set_status_text("Found no animations matching filter " + animationFilterStr);
						hasAnimationFilter = false;
					}
					else {
						set_status_text("Found " + std::to_string(nrMatches) + " animations matching filter ");
						i = 0;
						hasAnimationFilter = true;
						doAnimationLoop = true;
						break;
					}
				}
				else {
					hasAnimationFilter = false;
					break;
				}
			}
			else if (IsKeyDown(VK_ADD)) {//+
				log_to_file("UP");
				syncedAnimation.setDeltaZ(syncedAnimation.getDeltaZ() + 0.1);
				syncedAnimation.cleanupAfterExecution(true, true);
				syncedAnimation.executeSyncedAnimation(getActorPointers(), true, Vector3(), doAnimationLoop, true,0.0f);

				WAIT(150);
			}
			else if (IsKeyDown(VK_SUBTRACT)) {//-
				log_to_file("Down");
				syncedAnimation.setDeltaZ(syncedAnimation.getDeltaZ() - 0.1);
				syncedAnimation.cleanupAfterExecution(true, true);
				syncedAnimation.executeSyncedAnimation(getActorPointers(), true, Vector3(), doAnimationLoop, true, 0.0f);
				WAIT(150);
			}
			else if (IsKeyDown(0x41)) {//A
				log_to_file("Adding " + syncedAnimation.toString());
				action_animation_sync_add(syncedAnimation);
				set_status_text("Added synchronized animation to shortcut");
				WAIT(150);
			}

		}
		//cleanup objects and teleport back to start
		syncedAnimation.cleanupAfterExecution(true,true);

	}
	*/

	//reset cam
	CAM::RENDER_SCRIPT_CAMS(false, 0, 3000, 1, 0);
}



AnimationTrigger action_if_animation_sequence_shortcut_key_pressed(bool isRecording) {
	//ALT key
	if (IsKeyDown(VK_MENU)) {
		int animSequenceIndex = -1;
		if (IsKeyDown(VK_NUMPAD1)) {
			animSequenceIndex = 0;
		}
		else if (IsKeyDown(VK_NUMPAD2)) {
			animSequenceIndex = 1;
		}
		else if (IsKeyDown(VK_NUMPAD3)) {
			animSequenceIndex = 2;
		}
		else if (IsKeyDown(VK_NUMPAD4)) {
			animSequenceIndex = 3;
		}
		else if (IsKeyDown(VK_NUMPAD5)) {
			animSequenceIndex = 4;
		}
		else if (IsKeyDown(VK_NUMPAD6)) {
			animSequenceIndex = 5;
		}
		else if (IsKeyDown(VK_NUMPAD7)) {
			animSequenceIndex = 6;
		}
		else if (IsKeyDown(VK_NUMPAD8)) {
			animSequenceIndex = 7;
		}
		else if (IsKeyDown(VK_NUMPAD9)) {
			animSequenceIndex = 8;
		}
		else if (IsKeyDown(VK_NUMPAD0)) {
			animSequenceIndex = 9;
		}

		if (animSequenceIndex != -1) {

			if (animationSequences.size() > animSequenceIndex) {
				log_to_file("action_if_animation_sequence_shortcut_key_pressed");
				nextWaitTicks = 250;

				if (animationSequences[animSequenceIndex].isAnimationSequence()) {
					action_animation_sequence_play(animationSequences[animSequenceIndex].getAnimationSequence());
					return animationSequences[animSequenceIndex];
				}
				else if (animationSequences[animSequenceIndex].isSyncedAnimation()) {
					action_animation_sync_play(animationSequences[animSequenceIndex].getSyncedAnimation(),true);
					return animationSequences[animSequenceIndex];
				}



			}
			else {
				set_status_text("No animation sequence in slot " + std::to_string(animSequenceIndex + 1));
			}
		}
	}

	return AnimationTrigger::getNullAnimationTrigger();

}

void action_set_actor_as_passenger_of_other_actor(Actor passenger, Actor actorWithVehicle) {
	if (actorWithVehicle.isNullActor()) {
		set_status_text("Target actor does not exist");
		return;
	}

	Ped passengerPed = passenger.getActorPed();
	Ped pedWithVehicle = actorWithVehicle.getActorPed();

	if (passengerPed == pedWithVehicle) {
		set_status_text("Target actor is the same as current actor");
		return;
	}

	if (PED::IS_PED_IN_ANY_VEHICLE(pedWithVehicle, 0)) {
		Vehicle targetVehicle = PED::GET_VEHICLE_PED_IS_USING(pedWithVehicle);
		PED::SET_PED_INTO_VEHICLE(passengerPed, targetVehicle, -2);
	}
	else {
		set_status_text("Target actor is not in any vehicle");
	}
}

void check_if_actor_as_passenger_of_other_actor_key_pressed() {
	if (IsKeyDown(VK_PRIOR)) {
		int actorIndex = -1;
		if (IsKeyDown(0x31)) {
			//0 index is reserved for previously possessed or cloned
			actorIndex = 0;
		}
		else if (IsKeyDown(0x32)) {
			actorIndex = 1;
		}
		else if (IsKeyDown(0x33)) {
			actorIndex = 2;
		}
		else if (IsKeyDown(0x34)) {
			actorIndex = 3;
		}
		else if (IsKeyDown(0x35)) {
			actorIndex = 4;
		}
		else if (IsKeyDown(0x36)) {
			actorIndex = 5;
		}
		else if (IsKeyDown(0x37)) {
			actorIndex = 6;
		}
		else if (IsKeyDown(0x38)) {
			actorIndex = 7;
		}
		else if (IsKeyDown(0x39)) {
			actorIndex = 8;
		}
		if (actorIndex != -1) {
			Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
			action_set_actor_as_passenger_of_other_actor(actor, actors.at(actorIndex));
		}
		else {
			//do nothing
		}
	}
}

void check_if_explode_or_outofcontrol_nearby_vehicle_key_pressed (){
	if (IsKeyDown(VK_END) && IsKeyDown(VK_MENU)) {
		action_explode_nearby_vehicles(true);
	}else if (IsKeyDown(VK_END)) {
		action_explode_nearby_vehicles(false);
	}

}

void reset_ignore_player() {
	log_to_file("Setting PLAYER::SET_EVERYONE_IGNORE_PLAYER and PLAYER::SET_POLICE_IGNORE_PLAYER to false");
	
	PLAYER::SET_POLICE_IGNORE_PLAYER(PLAYER::PLAYER_PED_ID(), false);
	PLAYER::SET_EVERYONE_IGNORE_PLAYER(PLAYER::PLAYER_PED_ID(), false);

}

void action_add_prop_to_actor(Actor actor, ActorProp actorProp) {
	Ped actorPed = actor.getActorPed();

	Vector3 position = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(actorPed, 1.0, 2.0, 0.0);
	STREAMING::REQUEST_MODEL(GAMEPLAY::GET_HASH_KEY(actorProp.propId));
	DWORD ticksStart = GetTickCount();
	
	log_to_file("Loading prop with name " + actorProp.name + " (" + actorProp.propId + ")");

	while (!STREAMING::HAS_MODEL_LOADED(GAMEPLAY::GET_HASH_KEY(actorProp.propId))) {
		WAIT(0);

		if (GetTickCount() > ticksStart + 5000) {
			//duration will be 0 if it's not loaded
			set_status_text("Failed to load prop " + actorProp.name + " (" + actorProp.propId + ")");
			return;
		}
	}
		
	Object propObject = OBJECT::CREATE_OBJECT(GAMEPLAY::GET_HASH_KEY(actorProp.propId), position.x, position.y, position.z, 1, 1, 0);
	ENTITY::ATTACH_ENTITY_TO_ENTITY(propObject, actorPed, PED::GET_PED_BONE_INDEX(actorPed, actorProp.bone), 0.0, 0.15, 0.0, 0.0, -20.0, 180.0, 0, 0, 0, 0, 2, 1);
}


void action_toggle_scene_mode() {
	if (sceneMode == SCENE_MODE_ACTIVE) {
		sceneMode = SCENE_MODE_SETUP;
		set_status_text("Scene is now in setup mode. Press ALT+DEL to teleport actors back to start location");
		log_to_file("SCENE SETUP");
	}
	else {
		sceneMode = SCENE_MODE_ACTIVE;
		set_status_text("Scene is now active!");
		log_to_file("SCENE ACTIVE");
		create_relationship_groups();
	}

	

	//trigger the action for all actors in slots
	for (auto &actor: actors){
		actor.setActiveInScene(sceneMode);
		Ped actorPed = actor.getActorPed();


		//if not actor in slot, continue
		if (actor.isNullActor()) {
			continue;
		}

		ensure_ped_and_vehicle_is_not_deleted(actorPed);

		if (actor.isActiveInScene() == SCENE_MODE_ACTIVE) {
			//log_to_file("Actor " + std::to_string(i) + " Ped id:" + std::to_string(actorShortcut[i]) + " Has waypoint:"+ std::to_string(actorHasWaypoint[i])+  " Has start location:"+ std::to_string(actorHasStartLocation[i
			log_to_file("Actor " + std::to_string(actorPed) + " Has relationshipgroup " +actor.getRelationshipGroup().name);
			assign_actor_to_relationship_group(actorPed, actor.getRelationshipGroup());


			//store the current location of all actors without a recording, so that we can reset it
			if (!actor.hasRecording()) {
				actor.setStartLocation(ENTITY::GET_ENTITY_COORDS(actorPed, true));
				actor.setStartLocationHeading(ENTITY::GET_ENTITY_HEADING(actorPed));
				actor.setHasStartLocation(true);
			}

			if (actor.hasRecording()) {
				actor.startReplayRecording(GetTickCount());

			}
			else if (actor.hasWaypoint() && actor.isActorThisPed(PLAYER::PLAYER_PED_ID()) == false) { //move the actor if he has a waypoint and if he's not the player
			   //first if he's a driver
				move_to_waypoint(actorPed, actor.getWaypoint(), true);
				//second if he's a passenger
				check_if_ped_is_passenger_and_has_waypoint(actorPed);
				WAIT(200);
			}
		}
		else {
			actor.stopReplayRecording();
			ActorRecordingPlayback & recordingPlayback = actor.getRecordingPlayback();
			recordingPlayback.setRecordingItemIndex(0);


			//Disable default rel group
			//assign_actor_to_relationship_group(actorPed, getDefaultRelationshipGroup());
			//move the actor if he has a waypoint and if he's not the player
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actorPed, true);
			//AI::TASK_STAND_STILL(actorShortcut[i], -1);
			AI::CLEAR_PED_TASKS(actorPed);
			//AI::TASK_PAUSE(actorShortcut[i], 500);
		}
	}
	nextWaitTicks = 500;
}




bool record_scene_for_actor_key_press() {
	//ALT+ R
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x52)) {
		return true;
	}else if (IsKeyDown(VK_MENU) && IsKeyDown(0x45)) {
		return true;
	}
	else {
		return false;
	}
}

bool record_lipmovement_for_actor_key_press() {
	//J
	if (IsKeyDown(0x4A)) {
		return true;
	}
	else {
		return false;
	}
}

void action_stop_all_recording_replays() {
	for (auto & actor : actors) {
		if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
			actor.stopReplayRecording();
		}
	}
}

void action_start_replay_recording_for_actor(Actor & actor) {
	//action_teleport_to_start_locations();
	action_stop_all_recording_replays();
	actor.startReplayRecording(GetTickCount());

}

void action_set_recording_delay(Actor & actor) {

	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "FMMC_KEY_TIP8", "", strdup(std::to_string(actor.getRecordingDelay()).c_str()), "", "", "", 6);

	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		WAIT(0);
	}

	if (GAMEPLAY::IS_STRING_NULL_OR_EMPTY(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT())) {
		log_to_file("Got null keyboard value");
		return;
	}
	char * keyboardValue = GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT();
	try {
		int recordingDelay = std::stoi(keyboardValue);
		DWORD ticksDelay = (DWORD)recordingDelay;
		actor.setRecordingDelay(ticksDelay);
	}catch (std::exception &ex){
		log_to_file("Failed to parse " + std::string(keyboardValue));
		set_status_text("Failed to parse " + std::string(keyboardValue));
		return;
	}


}


/*Moved to Actor class
void update_tick_recording_replay(Actor actor) {
	Ped actorPed = actor.getActorPed();
	DWORD ticksNow = GetTickCount();

	//get the recording playback controller. Remember that this is by value and must be updated back to the actor
	ActorRecordingPlayback & recordingPlayback = actor.getRecordingPlayback();

	std::shared_ptr<ActorRecordingItem> recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
	std::shared_ptr<ActorRecordingItem> nextRecordingItem;
	if (!recordingPlayback.isCurrentRecordedItemLast()) {
		nextRecordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex() + 1);
	}
	std::shared_ptr<ActorRecordingItem> previousRecordingItem;
	if (recordingPlayback.getRecordedItemIndex()>=1) {
		previousRecordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex() -1);
	}


	if (!recordingPlayback.hasTeleportedToStartLocation()) {
		log_to_file("update_tick_recording_replay - Initiate telport to start location for all actors");
		action_teleport_to_start_locations();
		
		//update setting for all actors
		for (auto & actor : actors) {
			if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
				ActorRecordingPlayback & otherActorRecordingPlayback = actor.getRecordingPlayback();
				otherActorRecordingPlayback.setHasTeleportedToStartLocation(ticksNow);

				if (actor.hasRecordingWithGunFire() && actor.isActorThisPed(PLAYER::PLAYER_PED_ID())) {
					set_status_text("Switch actor in order for gun fire recording to be playbacked correctly!");
				}
			}
		}

		recordingPlayback.setHasTeleportedToStartLocation(ticksNow);
		return;
	}

	//special handling for the first item. Wait untill we start it
	if (!recordingPlayback.getHasFirstItemPlayback()) {
		
		if (ticksNow >= recordingPlayback.getTicksTeleportedToStartLocation() + 2000) {
			DWORD ticksPlaybackStart = recordingPlayback.getTicksPlaybackStarted();
			DWORD ticksDeltaStartFirst = recordingItem->getTicksAfterRecordStart();

			if (ticksNow < ticksPlaybackStart + ticksDeltaStartFirst + actor.getRecordingDelay()) {
				return;
			}
			else {
				recordingPlayback.setHasFirstItemPlayback(true);
				recordingPlayback.setTicksLastCheckOfCurrentItem(ticksNow);
				log_to_file("Starting first recording item");

				recordingItem->executeNativesForRecording(actor, nextRecordingItem, previousRecordingItem);
				
				//try to avoid flee and other actions
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actor.getActorPed(), true);
			}
		}
		else {
			return;
		}
	}

	//check for completion every recordingItem.getTicksDeltaCheckCompletion() ticks
	if (ticksNow >= recordingPlayback.getTicksLastCheckOfCurrentItem() + recordingItem->getTicksDeltaCheckCompletion()) {
		Vector3 currentLocation = ENTITY::GET_ENTITY_COORDS(actorPed, 1);
		//log_to_file(std::to_string(ticksNow) + " checking for completion ticks "+std::to_string(recordingPlayback.getTicksLastCheckOfCurrentItem() + recordingItem->getTicksDeltaCheckCompletion())+ " getticksdelta " + std::to_string(recordingItem->getTicksDeltaCheckCompletion()));
		log_to_file(std::to_string(ticksNow) + " checking for completion of item " + recordingItem->toString());






		if (recordingItem->isRecordingItemCompleted(nextRecordingItem,recordingPlayback.getTicksStartCurrentItem(), ticksNow, recordingPlayback.getAttemptsCheckedCompletion() , actor, currentLocation)) {
			//execute any post actions (normally empty)
			recordingItem->executeNativesAfterRecording(actor);

			//skip to next normally. But if last or ped is dead, skip to last
			if (recordingPlayback.isCurrentRecordedItemLast() || ENTITY::IS_ENTITY_DEAD(actorPed)) {
				recordingPlayback.setPlaybackCompleted();
				actor.stopReplayRecording();
			} 
			else {
				recordingPlayback.nextRecordingItemIndex(GetTickCount());
				recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
				log_to_file("Starting next recorded item " + std::to_string(recordingPlayback.getRecordedItemIndex())+ " : " + recordingItem->toString());
				recordingItem->executeNativesForRecording(actor, nextRecordingItem, previousRecordingItem);

				//try to avoid flee and other actions
				//PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actor.getActorPed(), true);
			}
		}
		else {
			recordingPlayback.setTicksLastCheckOfCurrentItem(ticksNow);
			recordingPlayback.incrementAttempstCheckedCompletion();
		}
	}

	//Logic for checking if current recording is ActorShootAtByImpactRecordingItem
	//std::shared_ptr<ActorShootAtByImpactRecordingItem> checkIfCurrentShootingRecordingItem = std::dynamic_pointer_cast<ActorShootAtByImpactRecordingItem>(recordingItem);
	//if (checkIfCurrentShootingRecordingItem) {
	//	log_to_file("Blocking mouse movements during ActorShootAtByImpactRecordingItem");
	//	CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(1);
	//}

}*/



/*
* Read keys from ini file
*/
void init_read_keys_from_ini() {
	GetPrivateProfileString("keys", "key_hud", "F10", key_hud_str, sizeof(key_hud_str), config_path);

	log_to_file("Read keys from ini file key_hud " + std::string(key_hud_str));

	key_hud = str2key(std::string(key_hud_str));
	if (key_hud == 0) {
		log_to_file(std::string(key_hud_str) + " is not a valid key");
		key_hud = str2key("F10");
	}

	log_to_file("Converted keys to dword key_possess " + std::to_string(key_hud));


	char key_menu_down_str[256], key_menu_up_str[256], key_menu_select_str[256], key_menu_left_str[256], key_menu_right_str[256], key_menu_delete_str[256];
	GetPrivateProfileString("keys", "key_menu_down", "NUM2", key_menu_down_str, sizeof(key_menu_down_str), config_path);
	GetPrivateProfileString("keys", "key_menu_up", "NUM8", key_menu_up_str, sizeof(key_menu_up_str), config_path);
	GetPrivateProfileString("keys", "key_menu_select", "NUM5", key_menu_select_str, sizeof(key_menu_select_str), config_path);
	GetPrivateProfileString("keys", "key_menu_left", "NUM4", key_menu_left_str, sizeof(key_menu_left_str), config_path);
	GetPrivateProfileString("keys", "key_menu_right", "NUM6", key_menu_right_str, sizeof(key_menu_right_str), config_path);
	GetPrivateProfileString("keys", "key_menu_delete", "DELETE", key_menu_delete_str, sizeof(key_menu_delete_str), config_path);


	key_menu_down = str2key(std::string(key_menu_down_str));
	if (key_menu_down == 0) {
		log_to_file(std::string(key_menu_down_str) + " is not a valid key");
		key_menu_down = str2key("NUM2");
	}

	key_menu_up = str2key(std::string(key_menu_up_str));
	if (key_menu_up == 0) {
		log_to_file(std::string(key_menu_up_str) + " is not a valid key");
		key_menu_up = str2key("NUM8");
	}

	key_menu_select = str2key(std::string(key_menu_select_str));
	if (key_menu_select == 0) {
		log_to_file(std::string(key_menu_select_str) + " is not a valid key");
		key_menu_select = str2key("NUM5");
	}

	key_menu_left = str2key(std::string(key_menu_left_str));
	if (key_menu_left == 0) {
		log_to_file(std::string(key_menu_left_str) + " is not a valid key");
		key_menu_left = str2key("NUM4");
	}

	key_menu_right = str2key(std::string(key_menu_right_str));
	if (key_menu_right == 0) {
		log_to_file(std::string(key_menu_right_str) + " is not a valid key");
		key_menu_right = str2key("NUM6");
	}

	key_menu_delete = str2key(std::string(key_menu_delete_str));
	if (key_menu_delete == 0) {
		log_to_file(std::string(key_menu_delete_str) + " is not a valid key");
		key_menu_delete = str2key("DELETE");
	}

}





bool enter_nearest_vehicle_as_passenger_key_pressed() {
	//ALT+E
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x46)) {
		return true;
	}
	else {
		return false;
	}
}

bool teleport_player_key_pressed() {
	//ALT+T
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x54)) {
		return true;
	}
	else {
		return false;
	}
}


bool scene_teleport_to_start_locations_key_pressed() {
	//ALT+END
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_DELETE)) {
		return true;
	}
	else {
		return false;
	}
}

bool set_same_waypoint_for_all_actors_key_pressed() {
	//ALT+DELETE
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_INSERT)) {
		return true;
	}
	else {
		return false;
	}
}

bool increase_aggressiveness_key_pressed() {
	//ALT+ NUMPAD+
	if (IsKeyDown(VK_ADD)) {
		return true;
	}
	else {
		return false;
	}
}

bool increase_aggressiveness_for_all_actors_key_pressed() {
	//ALT+ NUMPAD+
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_ADD)) {
		return true;
	}
	else {
		return false;
	}
}


bool decrease_aggressiveness_key_pressed() {
	//ALT+ NUMPAD-
	if (IsKeyDown(VK_SUBTRACT)) {
		return true;
	}
	else {
		return false;
	}
}

bool decrease_aggressiveness_for_all_actors_key_pressed() {
	//ALT+ NUMPAD-
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_SUBTRACT)) {
		return true;
	}
	else {
		return false;
	}
}

bool autopilot_for_player_key_pressed() {
	//ALT+ NUMPAD-
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x41)) {
		return true;
	}
	else {
		return false;
	}
}

bool vehicle_chase_key_pressed() {
	//ALT+ NUMPAD-
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_PRIOR)) {
		return true;
	}
	else {
		return false;
	}
}

bool vehicle_escort_key_pressed() {
	//ALT+ PageDown
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_NEXT)) {
		return true;
	}
	else {
		return false;
	}
}

bool reset_scene_director_key_pressed() {
	//ALT+ Pause/break
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_PAUSE)) {
		return true;
	}
	else {
		return false;
	}
}

bool copy_player_actions_key_pressed() {
	//ALT+ C
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x43)) {
		return true;
	}
	else {
		return false;
	}
}

void action_copy_recording_to_other_actors(Actor actorCopyFrom) {
	Ped actorCopyFromPed = actorCopyFrom.getActorPed();

	if (!actorCopyFrom.hasRecording()) {
		set_status_text("Actor has no recording to copy to others");
	}
	else {
		int nrCopiedTo = 0;
		std::vector<std::shared_ptr<ActorRecordingItem>> recordingToCopy = actorCopyFrom.getRecording();

		for (auto &actor : actors) {
			if (!actor.isActorThisPed(actorCopyFromPed) && !actor.isNullActor() && !actor.hasRecording()) {
				Ped actorPed = actor.getActorPed();
				actor.setRecording(recordingToCopy);
				actor.setHasRecording(true);

				actor.setStartLocation(ENTITY::GET_ENTITY_COORDS(actorPed, true));
				actor.setStartLocationHeading(ENTITY::GET_ENTITY_HEADING(actorPed));
				actor.setHasStartLocation(true);
				nrCopiedTo++;
			}
		}
		set_status_text("Copied actor recording to " + std::to_string(nrCopiedTo) + " other actors");

	}

}

void action_record_scene_for_actor(bool replayOtherActors) {

	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor()) {
		set_status_text("Actor must be assigned slot 1-9 before recording actions");
	}
	else {
		set_status_text("Recording actions for current actions. Press ALT+R to stop recording");
		Ped actorPed = actor.getActorPed();

		//the actual recording
		std::vector<std::shared_ptr<ActorRecordingItem>> actorRecording;
		actorRecording.reserve(1000);
		bool hasRecordingWithGunFire = false;

		//start other actors
		if (replayOtherActors) {
			for (auto &actor : actors) {
				actor.setActiveInScene(SCENE_MODE_ACTIVE);

				//if not actor in slot, continue
				if (actor.isNullActor()) {
					continue;
				}


				if (actor.isActiveInScene() == SCENE_MODE_ACTIVE) {
					//log_to_file("Actor " + std::to_string(i) + " Ped id:" + std::to_string(actorShortcut[i]) + " Has waypoint:"+ std::to_string(actorHasWaypoint[i])+  " Has start location:"+ std::to_string(actorHasStartLocation[i
					log_to_file("Actor " + std::to_string(actorPed) + " Has relationshipgroup " + actor.getRelationshipGroup().name);
					assign_actor_to_relationship_group(actorPed, actor.getRelationshipGroup());

					if (actor.hasRecording()) {
						actor.startReplayRecording(GetTickCount());
					}
				}
			}
		}


		//store current vehicle and seat
		Vehicle previousVehicle = 0;
		if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
			previousVehicle = PED::GET_VEHICLE_PED_IS_USING(actorPed);
			float previousVehicleHeading = ENTITY::GET_ENTITY_HEADING(previousVehicle);
			Vector3 previousVehicleLocation = ENTITY::GET_ENTITY_COORDS(previousVehicle, true);

			log_to_file("Actor has start location vehicle " + std::to_string(previousVehicle));
			actor.setStartLocationVehicle(previousVehicle, previousVehicleLocation, previousVehicleHeading);
			//find seat
			for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(previousVehicle); i++) {
				if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(previousVehicle, i) == actorPed) {
					actor.setStartLocationVehicleSeat(i);
					break;
				}
			}

		}

		float lastEntitySpeed = 0.0;
		Scenario currentScenario;
		DWORD ticksCurrentScenarioStart;
		bool isActorUsingScenario = false;
		std::shared_ptr<ActorScenarioRecordingItem> ongoingActorScenarioRecordingItem;

		bool isFreeAiming = false;
		bool isAimingAtEntity = false;
		Entity aimedAtEntity;
		DWORD ticksCurrentAimAtStart;
		std::shared_ptr<ActorAimAtRecordingItem> ongoingActorAimAtRecordingItem;

		bool isEnteringCover = false;
		std::shared_ptr<ActorCoverAtRecordingItem> ongoingActorCoverAtRecordingItem;
		DWORD ticksCurrentCoverStart;

		bool isReloading = false;
		bool isSpeaking = false;
		DWORD ticksStartedSpeaking = 0;
		DWORD ticksStoppedSpeaking = 0;
		

		DWORD lastBulletRecorded = 0;

		DWORD lastRocketBoost = 0;
		bool hasRecordedRocketBoost = false;

		bool hasRecordedVehParachute = false;
		Vehicle vehicleParachute = 0;

		//1. Store start location
		actor.setStartLocation(ENTITY::GET_ENTITY_COORDS(actorPed, true));
		actor.setStartLocationHeading(ENTITY::GET_ENTITY_HEADING(actorPed));
		actor.setHasStartLocation(true);

		WAIT(300);

		Vector3 actorLocation;

		bool bRecording = true;
		DWORD ticksStart = GetTickCount();
		DWORD ticksLast = ticksStart;
		DWORD ticksNow = ticksStart;
		DWORD ticksSinceStart;
		DWORD DELTA_TICKS = 1000;
		DWORD ticksNextControl=ticksNow;
		//4000 works well for boats

		//main loop
		while (bRecording == true) {
			ticksNow = GetTickCount();
			//CONTROLS::DISABLE_ALL_CONTROL_ACTIONS(0);
			//disable ALT key
			CONTROLS::DISABLE_CONTROL_ACTION(0, 19, 1);

			draw_instructional_buttons_player_recording();

			if (ticksNow >= ticksNextControl) {
				if (enter_nearest_vehicle_as_passenger_key_pressed()) {
					action_enter_nearest_vehicle_as_passenger();
					ticksNextControl = ticksNow + 200;
				}

				AnimationTrigger animationTriggerRecorded = action_if_animation_sequence_shortcut_key_pressed(true);
				ticksSinceStart = ticksNow - ticksStart;
				if (!animationTriggerRecorded.isNullAnimationTrigger() && animationTriggerRecorded.isAnimationSequence() && !animationTriggerRecorded.getAnimationSequence().isNullAnimationSequence()) {
					AnimationSequence animationSeqRecorded = animationTriggerRecorded.getAnimationSequence();
					
					actorLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);
					float heading = ENTITY::GET_ENTITY_HEADING(actorPed);
					ActorAnimationSequenceRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, heading,  animationSeqRecorded, animationFlag);
					log_to_file(recordingItem.toString());

					DELTA_TICKS = 500;

					std::bitset<19> bitCheck(animationFlag.id);
					log_to_file("Bitcheck " + bitCheck.to_string());
					if (!bitCheck.test(5)) {//Bit 6 is not set
						//log_to_file("Bitcheck 6");
						recordingItem.setTicksDeltaCheckCompletion((DWORD)animationSeqRecorded.getDurationOfAnimations());
						actorRecording.push_back(std::make_shared<ActorAnimationSequenceRecordingItem>(recordingItem));

						//take no new animation input before the animation is complete
						ticksNextControl = ticksNow + animationSeqRecorded.getDurationOfAnimations();
						//do no other recording before the animation is complete
						DELTA_TICKS = animationSeqRecorded.getDurationOfAnimations();
						log_to_file("ticksNow " + std::to_string(ticksNow) + "ticksNextControl " + std::to_string(ticksNextControl));

					}
					else {
						//log_to_file("Bitcheck !6");
						actorRecording.push_back(std::make_shared<ActorAnimationSequenceRecordingItem>(recordingItem));
						ticksNextControl = ticksNow + 250;
					}
	
					ticksLast = ticksNow;
				}
				else if (!animationTriggerRecorded.isNullAnimationTrigger() && animationTriggerRecorded.isSyncedAnimation() && !animationTriggerRecorded.getSyncedAnimation().isNull()) {
					//Synced animation
					SyncedAnimation syncedAnimation = animationTriggerRecorded.getSyncedAnimation();
					//action_animation_sync_play(syncedAnimation);
					
					//now store - Let's use a copy since it may be changed in edit scene
					SyncedAnimation* syncedAnimCopy = syncedAnimation.createCopy();
					//copy leaks memory (since on the heap). Keep references in case we want to free up
					syncedAnimationsInRecordings.push_back(syncedAnimCopy);

					syncedAnimCopy->clearObjectReferences();

					float actorRotation = ENTITY::GET_ENTITY_ROTATION(actorPed, 2).z;
					actorLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);

					ActorSyncedAnimationRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, getActorPointers(), actorLocation, actorRotation, syncedAnimCopy);
					
					//make sure we walk closer to where animation was recorded
					if (!PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
						float actorHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
						float walkSpeed = 1.0;
						if (AI::IS_PED_RUNNING(actorPed) || AI::IS_PED_SPRINTING(actorPed)) {
							walkSpeed = 2.0;
						}
						ActorOnFootMovementRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, walkSpeed, actorHeading);
						recordingItem.setMinDistanceBeforeCompleted(1.8);
						recordingItem.setTicksDeltaCheckCompletion(50);
						recordingItem.setNrAttemptsBeforeSkipping(50);
						actorRecording.push_back(std::make_shared<ActorOnFootMovementRecordingItem>(recordingItem));
						log_to_file(recordingItem.toString());
					}

					actorRecording.push_back(std::make_shared<ActorSyncedAnimationRecordingItem>(recordingItem));
					log_to_file(recordingItem.toString());
					ticksNextControl = ticksNow + syncedAnimation.getLength();
				}


			}


			//play recording for other actors if the scene mode is active
			if (replayOtherActors) {
				for (auto & actor : actors) {
					if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
						Actor::update_tick_recording_replay(actor);
					}
				}
			}


			//record only once pr DELTA_TICKS
			if (ticksNow - ticksLast >= DELTA_TICKS) {
				ticksSinceStart = ticksNow - ticksStart;

				actorLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);

				//Section if the actor is currently in a vehicle
				if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {

					//don't record as often for certain vehicles
					if (PED::IS_PED_IN_ANY_HELI(actorPed)) {
						DELTA_TICKS = 2000;
					}
					else if (PED::IS_PED_IN_ANY_PLANE(actorPed)) {
						DELTA_TICKS = 6000;
					}
					else if (PED::IS_PED_IN_ANY_BOAT(actorPed)) {
						DELTA_TICKS = 4000;
					}
					else if (PED::IS_PED_IN_ANY_SUB(actorPed)) {
						DELTA_TICKS = 3000;
					}
					else if (PED::IS_PED_ON_ANY_BIKE(actorPed)) {
						DELTA_TICKS = 1000;
					}
					else {
						DELTA_TICKS = 500;
					}

					Vehicle actorVeh = PED::GET_VEHICLE_PED_IS_USING(actorPed);
					float actorVehHeading = ENTITY::GET_ENTITY_HEADING(actorVeh);

					//check if it's a new vehicle
					if (previousVehicle != actorVeh) {

						float enterSpeed = 1.0;
						if (AI::IS_PED_RUNNING(actorPed)) {
							enterSpeed = 2.0;
						}
						int seat = -1;
						//find seat
						for (int i = -1; i < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(actorVeh); i++) {
							if (VEHICLE::GET_PED_IN_VEHICLE_SEAT(actorVeh, i) == actorPed) {
								seat = i;
								break;
							}
						}
						

						ActorVehicleEnterRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, actorVeh, actorVehHeading, seat, enterSpeed);
						actorRecording.push_back(std::make_shared<ActorVehicleEnterRecordingItem>(recordingItem));
						log_to_file(recordingItem.toString());
						previousVehicle = actorVeh;

					}
					else {//Existing vehicle movement
						float entitySpeed = ENTITY::GET_ENTITY_SPEED(actorVeh);
						float vehicleMaxSpeed = VEHICLE::_GET_VEHICLE_MAX_SPEED(ENTITY::GET_ENTITY_MODEL(actorVeh));
						float recordedSpeed = vehicleMaxSpeed;
						//if both previous and last speed is less than half max speed, we assume it's on purpose
						if (entitySpeed < (vehicleMaxSpeed / 2.0) && lastEntitySpeed < (vehicleMaxSpeed / 2.0)) {
							//make the speed the average of the last two
							recordedSpeed = (entitySpeed + lastEntitySpeed) / 2.0f;
						}
						//use vehicle location and not actor location
						Vector3 location = ENTITY::GET_ENTITY_COORDS(actorVeh, true);


						ActorVehicleMovementRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, location, actorVeh,actorVehHeading, recordedSpeed);
						actorRecording.push_back(std::make_shared<ActorVehicleMovementRecordingItem>(recordingItem));
						log_to_file(recordingItem.toString());

						lastEntitySpeed = entitySpeed;
					}
				}
				else {
					DELTA_TICKS = 1000;

					float previousVehicleHeading = ENTITY::GET_ENTITY_HEADING(previousVehicle);

					//Section if the actors is currently on foot
					//ActorVehicleExitRecordingItem
					//If actor was previously in a vehicle, the have exited. Record this
					if (previousVehicle != 0) {
						ActorVehicleExitRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, previousVehicle, previousVehicleHeading);
						actorRecording.push_back(std::make_shared<ActorVehicleExitRecordingItem>(recordingItem));
						log_to_file(recordingItem.toString());
						previousVehicle = 0;
					}
					else {
						//check for a scenario. Only apply if it's the first scenario or a new one
						if (PED::IS_PED_USING_ANY_SCENARIO(actorPed) && (isActorUsingScenario == false || PED::IS_PED_USING_SCENARIO(actorPed, currentScenario.name) == false)) {
							DELTA_TICKS = 500;
							isActorUsingScenario = true;
							//find scenario in use
							for (const auto scenario : gtaScenarios) {
								if (PED::IS_PED_USING_SCENARIO(actorPed, scenario.name)) {
									log_to_file("action_record_scene_for_actor: Found scenario player is using");
									log_to_file(scenario.name);
									currentScenario = scenario;
								}
							}
							log_to_file("action_record_scene_for_actor: Will add scenario");
							ticksCurrentScenarioStart = ticksNow;
							ActorScenarioRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, currentScenario);
							recordingItem.setTicksLength((DWORD)1000);
							ongoingActorScenarioRecordingItem = std::make_shared<ActorScenarioRecordingItem>(recordingItem);
							actorRecording.push_back(ongoingActorScenarioRecordingItem);
							log_to_file(recordingItem.toString());


						}
						else if (PED::IS_PED_USING_ANY_SCENARIO(actorPed)) {
							DELTA_TICKS = 500;
							log_to_file("action_record_scene_for_actor: Ped is still using the same scenario");
						}
						else if (PED::IS_PED_GOING_INTO_COVER(actorPed)) {
							if (!isEnteringCover) {
								isEnteringCover = true;
								ticksCurrentCoverStart = ticksNow;
								log_to_file("IS_PED_GOING_INTO_COVER");

								ActorCoverAtRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, actorLocation, actorLocation);
								recordingItem.setTicksLength((DWORD)1000);
								ongoingActorCoverAtRecordingItem = std::make_shared<ActorCoverAtRecordingItem>(recordingItem);
								actorRecording.push_back(ongoingActorCoverAtRecordingItem);
								log_to_file(recordingItem.toString());
							}

						}
						else if (PED::IS_PED_JUMPING(actorPed) || PED::IS_PED_CLIMBING(actorPed)) {
							float actorHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
							float walkSpeed = 1.0;
							if (AI::IS_PED_RUNNING(actorPed) || AI::IS_PED_SPRINTING(actorPed)) {
								walkSpeed = 2.0;
							}

							bool isClimbing = PED::IS_PED_CLIMBING(actorPed);
							if (isClimbing) {
								//walk over to where the jumping happened
								ActorOnFootMovementRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, walkSpeed, actorHeading);
								recordingItem.setMinDistanceBeforeCompleted(1.8);
								recordingItem.setTicksDeltaCheckCompletion(50);
								recordingItem.setNrAttemptsBeforeSkipping(50);
								actorRecording.push_back(std::make_shared<ActorOnFootMovementRecordingItem>(recordingItem));
								log_to_file(recordingItem.toString());
							}


							ActorJumpingRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, walkSpeed, actorHeading, isClimbing);
							if (isClimbing) {
								recordingItem.setTicksDeltaCheckCompletion(50);
							}
							else {
								//make sure jump is before going to next recording item
								recordingItem.setTicksDeltaCheckCompletion(400);
							}
							
							actorRecording.push_back(std::make_shared<ActorJumpingRecordingItem>(recordingItem));
							log_to_file(recordingItem.toString());
						}
						else {
							if (isActorUsingScenario) {//actor just stopped using scearion
								isActorUsingScenario = false;
								ongoingActorScenarioRecordingItem->setTicksLength(ticksNow - ticksCurrentScenarioStart);
								currentScenario = Scenario();
							}
							if (isEnteringCover) {
								isEnteringCover = false;
								log_to_file("Stopped entering cover for ActorCoverAtRecordingItem");
								ongoingActorCoverAtRecordingItem->setTicksLength(ticksNow - ticksCurrentCoverStart);
								ongoingActorCoverAtRecordingItem->setCoverPosition(actorLocation);
							}

							if (do_record_reload && PED::IS_PED_RELOADING(actorPed)) {
								if (isReloading == false) {//only record one reloading item pr reload
									isReloading = true;
									Hash weaponHash;
									WEAPON::GET_CURRENT_PED_WEAPON(actorPed, &weaponHash, 1);
									Vector3 weaponImpactLocation;
									bool doAim = false;
									if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(actorPed, &weaponImpactLocation)) {
										doAim = true;
									}

									ActorReloadRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, weaponHash, doAim, weaponImpactLocation);
									actorRecording.push_back(std::make_shared<ActorReloadRecordingItem>(recordingItem));
									log_to_file(recordingItem.toString());
								}
							}
							else {
								isReloading = false;
							}
							
							if (isReloading==false && PLAYER::IS_PLAYER_FREE_AIMING(PLAYER::PLAYER_ID())) {
								isFreeAiming = true;
								DELTA_TICKS = 10;
								Vector3 weaponImpactLocation;
								if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(actorPed, &weaponImpactLocation)) {
									Hash weaponHash;
									WEAPON::GET_CURRENT_PED_WEAPON(actorPed, &weaponHash, 1);

									DWORD deltaCheckRecordingTicks = 300;
									if (lastBulletRecorded != 0) {
										deltaCheckRecordingTicks = ticksNow - lastBulletRecorded;
										if (deltaCheckRecordingTicks > 1000) {
											deltaCheckRecordingTicks = 300;
										}
									}

									//check if there is movement whilst shooting
									float walkSpeed = 0.0;
									if (AI::IS_PED_RUNNING(actorPed) || AI::IS_PED_SPRINTING(actorPed)) {
										walkSpeed = 1.6;
									}
									else if (AI::IS_PED_WALKING(actorPed)) {
										walkSpeed =0.8;
									}
									else if (AI::IS_PED_STRAFING(actorPed)) {
										walkSpeed = 0.0;
									}

									float actorHeading = ENTITY::GET_ENTITY_HEADING(actorPed);

									//"FIRING_PATTERN_FULL_AUTO" FIRING_PATTERN_BURST_FIRE
									ActorShootAtByImpactRecordingItem recordingItem(ticksSinceStart, deltaCheckRecordingTicks, actorPed, actorLocation, weaponHash, weaponImpactLocation, GAMEPLAY::GET_HASH_KEY("FIRING_PATTERN_FULL_AUTO"), walkSpeed, actorHeading);
									actorRecording.push_back(std::make_shared<ActorShootAtByImpactRecordingItem>(recordingItem));
									log_to_file(recordingItem.toString());
									hasRecordingWithGunFire = true;

									lastBulletRecorded = ticksNow;
									//set accuracy to 100 for this ped
									PED::SET_PED_ACCURACY(actorPed, 100);
									//log_to_file("GET_PED_LAST_WEAPON_IMPACT_COORD (" + std::to_string(weaponImpactLocation.x) + "," + std::to_string(weaponImpactLocation.y) + "," + std::to_string(weaponImpactLocation.z) + ")");
								}
							}
							else if (isFreeAiming) {//finished free aiming. Stop recording
								isFreeAiming = false;
							}
							else {
								DELTA_TICKS = 1000;
								//record movement on foot
								float actorHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
								float walkSpeed = 1.0;
								if (AI::IS_PED_RUNNING(actorPed) || AI::IS_PED_SPRINTING(actorPed)) {
									walkSpeed = 2.0;
								}

								ActorOnFootMovementRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, walkSpeed, actorHeading);
								actorRecording.push_back(std::make_shared<ActorOnFootMovementRecordingItem>(recordingItem));

								log_to_file(recordingItem.toString());
							}


						}
					}

				}

				ticksLast = ticksNow;
			}



			if (record_scene_for_actor_key_press()) {
				bRecording = false;
			}
			else if (isSpeaking== false && record_lipmovement_for_actor_key_press() && ticksNow > ticksStoppedSpeaking+200) {
				isSpeaking = true;
				ticksStartedSpeaking = ticksNow;
				log_to_file("action_animation_lipmovement_start");
				action_animation_lipmovement_start();
			}
			else if (isSpeaking && record_lipmovement_for_actor_key_press() == false) {
				isSpeaking = false;
				log_to_file("action_animation_lipmovement_stop");
				action_animation_lipmovement_stop();
				ticksStoppedSpeaking = ticksNow;

				//record movement while speaking
				float actorHeading = ENTITY::GET_ENTITY_HEADING(actorPed);
				bool isWalkingWhileSpeaking = false;

				float walkSpeed = 0.0;
				if (AI::IS_PED_WALKING) {
					walkSpeed = 1.0;
					isWalkingWhileSpeaking = true;
				}else if (AI::IS_PED_RUNNING(actorPed) || AI::IS_PED_SPRINTING(actorPed)) {
					walkSpeed = 2.0;
					isWalkingWhileSpeaking = true;
				}

				ActorSpeakRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, walkSpeed, actorHeading, isWalkingWhileSpeaking);
				recordingItem.setTicksLength(ticksStoppedSpeaking - ticksStartedSpeaking);
				actorRecording.push_back(std::make_shared<ActorSpeakRecordingItem>(recordingItem));
				log_to_file(recordingItem.toString());
			}

			//check for rocket boost or vehicle parachute
			if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
				Vehicle actorVeh = PED::GET_VEHICLE_PED_IS_USING(actorPed);
				if (VEHICLE::_HAS_VEHICLE_ROCKET_BOOST(actorVeh) && VEHICLE::_IS_VEHICLE_ROCKET_BOOST_ACTIVE(actorVeh)){
					Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(actorVeh, -1);
					if (pedDriver == actorPed && (hasRecordedRocketBoost ==false || ticksSinceStart > lastRocketBoost + 5500)) {
						ActorVehicleRocketBoostRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, actorVeh, 0.0f);
						actorRecording.push_back(std::make_shared<ActorVehicleRocketBoostRecordingItem>(recordingItem));
						log_to_file(recordingItem.toString());
						lastRocketBoost = ticksSinceStart;
						hasRecordedRocketBoost = true;
					}
				}

				if (VEHICLE::_HAS_VEHICLE_PARACHUTE(actorVeh) && VEHICLE::_CAN_VEHICLE_PARACHUTE_BE_ACTIVATED(actorVeh)) {
					Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(actorVeh, -1);
					if (pedDriver == actorPed && hasRecordedVehParachute == false) {
						ActorVehicleParachuteRecordingItem recordingItem(ticksSinceStart, DELTA_TICKS, actorPed, actorLocation, actorVeh, 0.0f);
						actorRecording.push_back(std::make_shared<ActorVehicleParachuteRecordingItem>(recordingItem));
						log_to_file(recordingItem.toString());
						hasRecordedVehParachute = true;
						vehicleParachute = actorVeh;
					}
				}

			}

			

			WAIT(0);
		}
		log_to_file("Here are the recordings made for this actor:");
		for (auto recordingItem : actorRecording) {
			log_to_file(recordingItem->toString());
		}

		if (actorRecording.size() > 0) {
			actor.setRecording(actorRecording);
			actor.setHasRecording(true);
			actor.setHasRecordingWithGunFire(hasRecordingWithGunFire);
		}


		log_to_file("Recorded " + std::to_string(actorRecording.size()) + " instructions");
		set_status_text("Recorded " + std::to_string(actorRecording.size()) + " instructions");

		//doesn't have the intended effect
		if (hasRecordedVehParachute) {
			VEHICLE::_SET_VEHICLE_PARACHUTE_ACTIVE(vehicleParachute, false);
		}

		set_status_text("Recording stopped");
		nextWaitTicks = 400;

	}
}

void action_submenu_active_selected() {
	log_to_file("action_submenu_active_selected " + std::to_string(submenu_active_action));
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	//switch to actor
	if (submenu_active_action == SUBMENU_ITEM_RECORD_PLAYER) {
		action_record_scene_for_actor(false);
	}
	else if (submenu_active_action == SUBMENU_ITEM_RECORD_PLAYER_WOTHERS) {
		action_record_scene_for_actor(true);
	}
	else if (submenu_active_action == SUBMENU_ITEM_TEST_RECORDING) {
		log_to_file("Will test recording for " + std::to_string(actor.getActorPed()));
		action_start_replay_recording_for_actor(actor);
	}
	else if (submenu_active_action == SUBMENU_ITEM_RECORDING_DELAY) {
		action_set_recording_delay(actor);
	}
	else if (submenu_active_action == SUBMENU_ITEM_IS_PLAYING_RECORDING) {
		log_to_file("Skipping to next recording item since user pressed meny");

		ActorRecordingPlayback & recordingPlayback = actor.getRecordingPlayback();
		//skip to next or end if this is the last 
		if (recordingPlayback.isCurrentRecordedItemLast()) {
			recordingPlayback.setPlaybackCompleted();
			actor.stopReplayRecording();
		}
		else {
			recordingPlayback.nextRecordingItemIndex(GetTickCount());
			std::shared_ptr<ActorRecordingItem> recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
			std::shared_ptr<ActorRecordingItem> nextRecordingItem;
			if (!recordingPlayback.isCurrentRecordedItemLast()) {
				nextRecordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex() + 1);
			}
			std::shared_ptr<ActorRecordingItem> previousRecordingItem;
			if (recordingPlayback.getRecordedItemIndex() >= 1) {
				previousRecordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex() - 1);
			}

			log_to_file("Starting next recorded item " + std::to_string(recordingPlayback.getRecordedItemIndex()) + " : " + recordingItem->toString());
			recordingItem->executeNativesForRecording(actor, nextRecordingItem, previousRecordingItem);
		}

	}
	else if (submenu_active_action == SUBMENU_ITEM_DELETE_RECORDING) {
		log_to_file("Will remove recording for " + std::to_string(actor.getActorPed()));
		actor.stopReplayRecording();
		actor.removeRecording();
	}
	else if (submenu_active_action == SUBMENU_ITEM_REMOVE_FROM_SLOT) {
		//menu_active_action is the index of the actor from the main menu
		action_remove_actor_from_index(menu_active_action);
		submenu_is_displayed = false;
	}
	else if (submenu_active_action == SUBMENU_ITEM_COPY_REC_TO_OTHERS) {
		action_copy_recording_to_other_actors(actor);
	}
	else if (submenu_active_action == SUBMENU_ITEM_BLACKOUT) {
		action_toggle_blackout();
	}
	else if (submenu_active_action == SUBMENU_ITEM_TIMELAPSE) {
		action_toggle_timelapse();
	}
	else if (submenu_active_action == SUBMENU_ITEM_WEATHER) {
		action_next_weather();
	}
	else if (submenu_active_action == SUBMENU_ITEM_WIND) {
		action_toggle_wind();
	}
	else if (submenu_active_action == SUBMENU_ITEM_RECORD_RELOAD) {
		do_record_reload = !do_record_reload;
	}
	else if (submenu_active_action == SUBMENU_ITEM_CHASE) {
		action_vehicle_chase();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ESCORT) {
		action_vehicle_escort();
	}
	else if (submenu_active_action == SUBMENU_ITEM_SPOT_LIGHT) {
		action_next_spot_light();
	}
	else if (submenu_active_action == SUBMENU_ITEM_SPOT_LIGHT_COLOR) {
		action_next_spot_light_color();
	}
	else if (submenu_active_action == SUBMENU_ITEM_WALK) {
		action_next_walking_style();
	}
	else if (submenu_active_action == SUBMENU_ITEM_WALK_SPEED) {
		action_next_walking_speed();
	}
	else if (submenu_active_action == SUBMENU_ITEM_DRIVING_MODE) {
		action_next_driving_mode();
	}
	else if (submenu_active_action == SUBMENU_ITEM_EDIT_NAME) {
		action_edit_name();
	}
	else if (submenu_active_action == SUBMENU_ITEM_RELATIONSHIP) {
		action_next_relationshipgroup();
	}
	else if (submenu_active_action == SUBMENU_ITEM_HEALTH) {
		action_next_health();
	}
	else if (submenu_active_action == SUBMENU_ITEM_VEHICLE_COSMETIC) {
		action_toggle_vehicle_cosmetic();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ANIMATION_PREVIEW) {
		action_animations_preview();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ANIMATION_SINGLE) {
		action_animation_single();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ANIMATION_FLAG) {
		action_next_animation_flag();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ANIMATION_SEQUENCE) {
		action_animation_sequence_add();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ANIMATION_SYNC) {
		action_animation_sync_setup();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ANIMATION_SYNC_PREVIEW) {
		action_animation_sync_preview();
		menu_active_index = 0;
		submenu_active_index = 0;
	}
	else if (submenu_active_action >= SUBMENU_ITEM_ANIMATION_SEQUENCE_0 && submenu_active_action <= SUBMENU_ITEM_ANIMATION_SEQUENCE_20) {
		int animSequenceIndex = submenu_active_action - SUBMENU_ITEM_ANIMATION_SEQUENCE_0;
		if (animSequenceIndex >= 0 && animSequenceIndex < animationSequences.size()) {
			if (animationSequences[animSequenceIndex].isAnimationSequence()) {
				action_animation_sequence_play(animationSequences[animSequenceIndex].getAnimationSequence());
			}
			else if (animationSequences[animSequenceIndex].isSyncedAnimation()) {
				action_animation_sync_play(animationSequences[animSequenceIndex].getSyncedAnimation(),true);
			}

		}
	}
	else if (submenu_active_action == SUBMENU_ITEM_ACTOR_PROP_SELECT) {
		action_next_actor_prop();
	}
	else if (submenu_active_action == SUBMENU_ITEM_ACTOR_PROP_ADD) {
		action_add_prop_to_actor(actor, currentActorProp);
	}
	else if (submenu_active_action == SUBMENU_ITEM_SAVE_ACTORS) {
		action_save_actors();
	}
	else if (submenu_active_action == SUBMENU_ITEM_LOAD_ACTORS) {
		action_load_actors();
	}

	

}


void action_submenu_active_delete() {
	log_to_file("action_submenu_active_delete " + std::to_string(submenu_active_action));
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	Ped actorPed = actor.getActorPed();

	if (submenu_active_action >= SUBMENU_ITEM_ANIMATION_SEQUENCE_0 && submenu_active_action <= SUBMENU_ITEM_ANIMATION_SEQUENCE_20) {
		int animSequenceIndex = submenu_active_action - SUBMENU_ITEM_ANIMATION_SEQUENCE_0;
		if (animSequenceIndex >= 0 && animSequenceIndex < animationSequences.size()) {
			animationSequences.erase(animationSequences.begin() + animSequenceIndex);
		}
	}else if (submenu_active_action == SUBMENU_ITEM_IS_PLAYING_RECORDING) {
		log_to_file("Skipping to next recording item since user pressed meny");

		ActorRecordingPlayback & recordingPlayback = actor.getRecordingPlayback();
		std::shared_ptr<ActorRecordingItem> recordingItem = actor.getRecordingAt(recordingPlayback.getRecordedItemIndex());
		Vector3 targetLocation = recordingItem->getLocation();
		Entity entityToTeleport = actorPed;
		if (PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0)) {
			entityToTeleport = PED::GET_VEHICLE_PED_IS_USING(actorPed);
			if (VEHICLE::IS_VEHICLE_STUCK_ON_ROOF(entityToTeleport)) {
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(entityToTeleport);
				WAIT(100);
			}

		}
		teleport_entity_to_location(entityToTeleport, targetLocation, true);

	}




}

void action_menu_active_delete() {
	log_to_file("action_menu_active_delete " + std::to_string(menu_active_action));
	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());

	if (menu_active_action >= 1 && menu_active_action <= 100) {
		int actorIndex = menu_active_action-1;
		if (actorIndex >= 0 && actorIndex < actors.size()) {
			actors[actorIndex] = Actor::nullActor();
			//force the active menu to be set to the new actor
			menu_active_index = -1;
			menu_active_ped = PLAYER::PLAYER_PED_ID();
		}

	}
}


void action_menu_active_selected() {
	log_to_file("action_menu_active_selected " + std::to_string(menu_active_action));
	//switch to actor
	if (menu_active_action >= 1 && menu_active_action <= 100) {
		action_swap_to_actor_with_index(menu_active_action);
		//force the active menu to be set to the new actor
		menu_active_index = -1;
		menu_active_ped = PLAYER::PLAYER_PED_ID();
	}
	else if (menu_active_action == MENU_ITEM_AUTOPILOT) {
		//autpilot is cancelled by switching to current actor
		possess_ped(PLAYER::PLAYER_PED_ID());
	}
	else if (menu_active_action == MENU_ITEM_BIRDS_EYE_MODE) {
		log_to_file("Switching to Birds eye mode");
		activeMode = MODE_BIRDS_EYE_VIEW;
		birdsEyeController.onEnterMode(sceneMode);
	}
	else if (menu_active_action == MENU_ITEM_SCENE_MODE) {
		action_toggle_scene_mode();
	}
	else if (menu_active_action == MENU_ITEM_BACK_TO_START) {
		action_stop_all_recording_replays();
		action_teleport_to_start_locations();
	}
	else if (menu_active_action == MENU_ITEM_ADD_TO_SLOT) {
		add_ped_to_slot(get_next_free_slot(), PLAYER::PLAYER_PED_ID());
		//force the active menu to be set to the new actor
		menu_active_index = -1;
		menu_active_ped = PLAYER::PLAYER_PED_ID();
	}
	else if (menu_active_action == MENU_ITEM_ADD_CLONE_TO_SLOT && actor0IsClone) {
		if (get_actor_from_ped(previousActor.getActorPed()).isNullActor() == true) {
			int slot = get_next_free_slot();
			if (slot != -1) {
				add_ped_to_slot(slot, previousActor.getActorPed());
				actor0IsClone = false;
				action_swap_to_actor_with_index(slot);
				//force the active menu to be set to the new actor
				menu_active_index = -1;
			}
		}
	}
	else if (menu_active_action == MENU_ITEM_CLONE) {
		action_clone_player();
		menu_active_index = 0;
	}
	else if (menu_active_action == MENU_ITEM_CLONE_WITH_VEHICLE) {
		action_clone_player_with_vehicle();
		menu_active_index = 0;
	}
	else if (menu_active_action == MENU_ITEM_POSSESS) {
		action_possess_ped();
		menu_active_index = -1;
	}
	else if (menu_active_action == MENU_ITEM_FIRING_SQUAD) {
		is_firing_squad_engaged = false;
	}

}

void action_copy_player_actions() {

	Actor & actor = get_actor_from_ped(PLAYER::PLAYER_PED_ID());
	if (actor.isNullActor()) {
		set_status_text("Actor must be assigned slot 1-9 before recording actions");
	}
	else {
		is_firing_squad_engaged = true;
		set_status_text("Firing squad mode: Actors will now copy the actions of the player");
		Ped playerPed = actor.getActorPed();

		WAIT(500);

		bool bCopying = true;
		DWORD tickStart = GetTickCount();
		DWORD tickLast = tickStart;
		DWORD tickNow = tickStart;
		CONST DWORD DELTA_TICKS = 10;
		Hash previousWeapon;
		bool isFreeAiming = false;
		bool isShooting = false;
		Entity currentTarget;
		Vehicle lastVehicle;
		bool hasGivenParachute = false;
		bool isSkydiving = false;

		bool isPedUsingScenario = false;
		Scenario currentScenario;
		//try to avoid them fleeing on gunshots
		for (auto &actor : actors) {
			if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actor.getActorPed(), true);
			}
		}


		//main loop
		while (is_firing_squad_engaged) {
			tickNow = GetTickCount();

			//Display HUD for app on every tick
			if (should_display_app_hud()) {
				draw_instructional_buttons();
				draw_menu();
			}

			//check if we're waiting nextWaitTicks number of ticks
			if (nextWaitTicks != 0 && tickNow - tickLast < nextWaitTicks) {
				//do nothing 

			}
			else if (tickNow - tickLast >= DELTA_TICKS) {//check only once pr DELTA_TICKS
				if (playerPed != PLAYER::PLAYER_PED_ID()) {
					playerPed = PLAYER::PLAYER_PED_ID();
					log_to_file("Swapping players during firing squad mode. Not sure if this will work well");
					previousWeapon = 0;
					hasGivenParachute = false;
					lastVehicle = 0;
					currentTarget = 0;
				}

				nextWaitTicks = 0;


				Vector3 actorLocation = ENTITY::GET_ENTITY_COORDS(playerPed, true);

				if (hud_toggle_key_pressed()) {
					if (should_display_hud == true) {
						should_display_hud = false;
					}
					else {
						should_display_hud = true;
					}
					nextWaitTicks = 100;
				}

				//Copy scenarios applied to player to other actors
				//No way of retrieving which scenario is active, so we have to brute force it over the 458 scenarios which exist
				if (PED::IS_PED_USING_ANY_SCENARIO(playerPed)) {
					
					bool applyCurrentScenarioToActors = false;
					if (isPedUsingScenario == false || (isPedUsingScenario == true && PED::IS_PED_USING_SCENARIO(playerPed, currentScenario.name) == false)) {
						log_to_file("Ped is using new scenario");
						isPedUsingScenario = true;

						for (const auto scenario : gtaScenarios) {
							if (PED::IS_PED_USING_SCENARIO(playerPed, scenario.name)) {
								log_to_file("found scenario player is using");
								log_to_file(scenario.name);
								currentScenario = scenario;
								applyCurrentScenarioToActors = true;
							}
						}
					}

					if (applyCurrentScenarioToActors) {
						log_to_file("Applying scenario to actors");
						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
								if (currentScenario.hasEnterAnim) {
									AI::TASK_START_SCENARIO_IN_PLACE(actor.getActorPed(), currentScenario.name, -1, 1);
								}
								else {
									AI::TASK_START_SCENARIO_IN_PLACE(actor.getActorPed(), currentScenario.name, -1, 0);
								}
								
							}
						}
					}
				}
				else if (isPedUsingScenario) {
					log_to_file("Removing scenario from actors");
					isPedUsingScenario = false;

					for (auto &actor : actors) {
						if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
							AI::CLEAR_PED_TASKS(actor.getActorPed());
						}
					}
				}


				//check if the player is armed
				if (WEAPON::IS_PED_ARMED(playerPed, 6)) {
					Hash currentWeapon;
					WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &currentWeapon, 1);
					if (currentWeapon != previousWeapon) {
						previousWeapon = currentWeapon;
						//give and equip weapon to all other actors
						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
								if (isSkydiving == false) {
									log_to_file("Giving weapon " + std::to_string(currentWeapon) + " to actor " + std::to_string(actor.getActorPed()));
									WEAPON::GIVE_WEAPON_TO_PED(actor.getActorPed(), currentWeapon, 1000, 1, 1);
								}
								else {
									log_to_file("Not giving weapon to actor as we are skydiving");
								}
							}
						}
					}
				}

				//check if the player is aiming or firing
				if (PLAYER::IS_PLAYER_FREE_AIMING(PLAYER::PLAYER_ID())) {
					isFreeAiming = true;
					Entity targetEntity;
					bool playerIsAimingAtEntity = PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(PLAYER::PLAYER_ID(), &targetEntity);
					log_to_file("Player aiming at " + std::to_string(targetEntity));
					//if new target which exist, make all actors aim at it
					if (playerIsAimingAtEntity && targetEntity != currentTarget) {
						currentTarget = targetEntity;
						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {

								if (isSkydiving == false) {//TASK_AIM_GUN_AT_ENTITY while skydiving has a tiny sideeffect
														   //log_to_file("Aim at " + std::to_string(targetEntity) + " for actor " + std::to_string(actorShortcut[i]));
									if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
										//doesn't work :(
										AI::TASK_VEHICLE_AIM_AT_PED(lastVehicle, targetEntity);

									}
									else {
										AI::TASK_AIM_GUN_AT_ENTITY(actor.getActorPed(), targetEntity, -1, 0);
									}

								}
							}
						}
					}

					if (PED::IS_PED_SHOOTING(playerPed)) {
						isShooting = true;
						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false && actor.isActorThisPed(currentTarget)==false) {
								//log_to_file("Shoot at " + std::to_string(targetEntity) + " for actor " + std::to_string(actorShortcut[i]));
								if (isSkydiving == false) {//TASK_SHOOT_AT_ENTITY while skydiving has a tiny sideeffect
									if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
										//doesn't work :(
										AI::TASK_VEHICLE_SHOOT_AT_PED(lastVehicle, targetEntity, 0x41a00000);
									}
									else {
										AI::TASK_SHOOT_AT_ENTITY(actor.getActorPed(), currentTarget, -1, GAMEPLAY::GET_HASH_KEY("FIRING_PATTERN_SINGLE_SHOT"));
									}
								}
							}
						}
						//WAIT(50);
						nextWaitTicks = 50;
					}
					else if (isShooting) {//make actors stop shooting
						isShooting = false;
						currentTarget = 0;

					}


				}
				else if (isFreeAiming) {//make actors peds stop aiming
					for (auto &actor : actors) {
						if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {

							if (isSkydiving == false) {//clear_ped_tasks while skydiving has a tiny sideeffect
								log_to_file("Clearing tasks for actor " + std::to_string(actor.getActorPed()));
								AI::CLEAR_PED_TASKS(actor.getActorPed());
							}

						}
					}
					currentTarget = 0;
					isFreeAiming = false;
					isShooting = false;
				}

				//check if the player is in a vehicle
				if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
					Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);

					if (pedVehicle != lastVehicle) {
						isSkydiving = false;
						hasGivenParachute = false;
						lastVehicle = pedVehicle;
						log_to_file("Actors should enter vehicle " + std::to_string(pedVehicle));
						int seatIndex = 0;
						if (PED::IS_PED_IN_ANY_PLANE(playerPed)) {
							log_to_file("In plane with seats " + std::to_string(VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(pedVehicle)));
						}

						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
								log_to_file("Actor " + std::to_string(actor.getActorPed()) + " should enter vehicle " + std::to_string(pedVehicle) + " in seat " + std::to_string(seatIndex));

								if (PED::IS_PED_IN_ANY_PLANE(playerPed)) {
									PED::SET_PED_INTO_VEHICLE(actor.getActorPed(), pedVehicle, -2);
								}
								else {
									AI::TASK_ENTER_VEHICLE(actor.getActorPed(), pedVehicle, -1, seatIndex, 1.0, 1, 0);
								}
								seatIndex++;
							}
						}

					}
				}

				//check if the player is falling
				if (PED::GET_PED_PARACHUTE_STATE(playerPed) == 0) {
					if (hasGivenParachute == false) {
						log_to_file("Everyone leave vehicle");
						//AI::TASK_EVERYONE_LEAVE_VEHICLE(lastVehicle);

						log_to_file("Giving parachutes to all actors and task sky dive");
						hasGivenParachute = true;
						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
								WEAPON::GIVE_WEAPON_TO_PED(actor.getActorPed(), GAMEPLAY::GET_HASH_KEY("gadget_parachute"), 1, 1, 1);
								//AI::TASK_SKY_DIVE(actorShortcut[i]);
								AI::TASK_LEAVE_VEHICLE(actor.getActorPed(), lastVehicle, 4160);
							}
						}
					}

				}
				else if (PED::GET_PED_PARACHUTE_STATE(playerPed) == 1 || PED::GET_PED_PARACHUTE_STATE(playerPed) == 2) {

					if (isSkydiving == false) {
						isSkydiving = true;
						log_to_file("Actors should deploy parachute");
					}
					else {//isSkydiving = true;
						Vector3 playerLocation = ENTITY::GET_ENTITY_COORDS(playerPed, true);
						float zGroundLevel;
						GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(playerLocation.x, playerLocation.y, playerLocation.z, &zGroundLevel,0);

						//log_to_file("Ground level to parachute to is " +std::to_string(zGroundLevel));

						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false) {
								if (PED::GET_PED_PARACHUTE_STATE(actor.getActorPed()) == 0) {
									//log_to_file("Actor " + std::to_string(actorShortcut[i]) + " should deploy parachute");

									AI::TASK_PARACHUTE_TO_TARGET(actor.getActorPed(), playerLocation.x, playerLocation.y, zGroundLevel);

								}
								else if (PED::IS_PED_IN_ANY_VEHICLE(actor.getActorPed(), 0)) {
									//log_to_file("Actor " + std::to_string(actorShortcut[i]) + " should sky dive");
									//had some issues with them not exiting the vehicle so use TASK_LEAVE_VEHICLE with a teleport flag instead
									//AI::TASK_SKY_DIVE(actorShortcut[i]);
									AI::TASK_LEAVE_VEHICLE(actor.getActorPed(), lastVehicle, 4160);
									WEAPON::GIVE_WEAPON_TO_PED(actor.getActorPed(), GAMEPLAY::GET_HASH_KEY("gadget_parachute"), 1, 1, 1);
								}
							}
						}
						WAIT(50);
					}
				}
				else if (isSkydiving) {//check if we have landed
					if (PED::GET_PED_PARACHUTE_STATE(playerPed) == -1) {
						bool allActorsOnGround = true;
						for (auto &actor : actors) {
							if (actor.isNullActor() == false && actor.isActorThisPed(playerPed) == false && PED::GET_PED_PARACHUTE_STATE(actor.getActorPed()) != -1) {
								//log_to_file("Actor " + std::to_string(actorShortcut[i]) + " has not yet landed");
								allActorsOnGround = false;
							}
						}
						if (allActorsOnGround) {
							log_to_file("All actors has landed. Skydiving is finished");
							isSkydiving = false;
							hasGivenParachute = false;
						}
					}

				}

				if (should_display_app_hud()) {
					if (menu_up_key_pressed()) {
						menu_action_up();
					}
					else if (menu_down_key_pressed()) {
						menu_action_down();
					}
					else if (menu_left_key_pressed()) {
						menu_action_left();
					}
					else if (menu_right_key_pressed()) {
						menu_action_right();
					}
					else if (menu_select_key_pressed()) {
						menu_action_select();
					}
				}

				tickLast = tickNow;

				if (copy_player_actions_key_pressed()) {
					is_firing_squad_engaged = false;
				}
			}

			WAIT(0);
		}
		is_firing_squad_engaged = false;
		set_status_text("Firing squad mode stopped");
		nextWaitTicks = 400;

	}


}


bool menu_up_key_pressed() {
	if (IsKeyDown(key_menu_up)) {
		return true;
	}
	else {
		return false;
	}
}

bool menu_down_key_pressed() {
	if (IsKeyDown(key_menu_down)) {
		return true;
	}
	else {
		return false;
	}
}

bool menu_left_key_pressed() {
	if (IsKeyDown(key_menu_left)) {
		return true;
	}
	else {
		return false;
	}
}

bool menu_right_key_pressed() {
	if (IsKeyDown(key_menu_right)) {
		return true;
	}
	else {
		return false;
	}
}


bool menu_select_key_pressed() {
	if (IsKeyDown(key_menu_select)) {
		return true;
	}
	else {
		return false;
	}
}

bool menu_delete_key_pressed() {
	if (IsKeyDown(key_menu_delete)) {
		return true;
	}
	else {
		return false;
	}
}

bool menu_back_key_pressed() {
	if (IsKeyDown(key_menu_back)) {
		return true;
	}
	else {
		return false;
	}
}



void menu_action_up() {
	if (submenu_is_active == false) {
		menu_active_index++;
		if (menu_active_index > menu_max_index) {
			menu_active_index = 0;
			nextWaitTicks = 200;
		}
		else {
			submenu_is_displayed = false;
			submenu_is_active = false;
			submenu_active_index = -1;
		}
	}
	else {
		submenu_active_index++;
		if (submenu_active_index > submenu_max_index) {
			submenu_active_index = submenu_max_index;
		}
	}
	nextWaitTicks = 100;
}


//alternative menu starts at the top and goes down
void menu_alt_action_down() {
	if (submenu_is_active == false) {
		menu_active_index++;
		if (menu_active_index > menu_max_index) {
			log_to_file("menu_active_index > menu_max_index. Setting menu_active_index = 0");
			menu_active_index = 0;
		}
	}
}

//alternative menu starts at the top and goes down
void menu_alt_action_up() {
	if (submenu_is_active == false) {
		menu_active_index--;
		if (menu_active_index < 0) {
			log_to_file("menu_active_index < 0. Setting menu_active_index = menu_max_index");
			menu_active_index = menu_max_index;
		}
	}
}


void menu_action_down() {
	if (submenu_is_active == false) {
		menu_active_index--;
		if (menu_active_index < 0) {
			menu_active_index = menu_max_index;
			nextWaitTicks = 200;
		}
		else {
			submenu_is_displayed = false;
			submenu_is_active = false;
			submenu_active_index = -1;
		}
	}
	else {
		submenu_active_index--;
		if (submenu_active_index < 0) {
			submenu_active_index = 0;
		}
	}
	nextWaitTicks = 100;
}

void menu_action_left() {
	if (submenu_is_displayed) {
		submenu_is_active = true;
		submenu_active_index = 0;
	}
}

void menu_action_right() {
	if (submenu_is_displayed) {
		submenu_is_active = false;
	}
}

void menu_action_select() {
	if (submenu_is_active) {
		action_submenu_active_selected();
	}
	else {
		action_menu_active_selected();
	}
	nextWaitTicks = 200;
}

void menu_action_delete() {
	if (submenu_is_active) {
		action_submenu_active_delete();
	}
	else {
		action_menu_active_delete();
	}
	nextWaitTicks = 200;
}




void main()
{


	while (true)
	{

		if (activeMode == MODE_STANDARD) {

			//TODO: Refactor standard mode into separate class in same way as BirdsEyeMode

			/* ACTIONS WHICH MAY NEED TO WAIT A FEW TICKS */
			if (nextWaitTicks == 0 || GetTickCount() - mainTickLast >= nextWaitTicks) {
				//nextWaitTicks will be set by action methods in order to define how long before next input can be processed
				nextWaitTicks = 0;

				if (scene_teleport_to_start_locations_key_pressed()) {
					action_stop_all_recording_replays();
					action_teleport_to_start_locations();
				}


				if (set_same_waypoint_for_all_actors_key_pressed()) {
					action_set_same_waypoint_for_all_actors();
				}

				if (hud_toggle_key_pressed()) {
					if (should_display_hud == true) {
						should_display_hud = false;
					}
					else {
						action_show_info_on_start();
						should_display_hud = true;
					}
				}


				if (autopilot_for_player_key_pressed()) {
					action_autopilot_for_player(false);
				}

				if (vehicle_chase_key_pressed()) {
					action_vehicle_chase();
				}

				if (vehicle_escort_key_pressed()) {
					action_vehicle_escort();
				}

				if (reset_scene_director_key_pressed()) {
					action_reset_scene_director();
				}

				if (copy_player_actions_key_pressed()) {
					action_copy_player_actions();
				}

				if (enter_nearest_vehicle_as_passenger_key_pressed()) {
					action_enter_nearest_vehicle_as_passenger();
				}

				if (teleport_player_key_pressed()) {
					teleport_player_to_waypoint();
				}

				if (should_display_app_hud()) {
					if (menu_up_key_pressed()) {
						menu_action_up();
					}
					else if (menu_down_key_pressed()) {
						menu_action_down();
					}
					else if (menu_left_key_pressed()) {
						menu_action_left();
					}
					else if (menu_right_key_pressed()) {
						menu_action_right();
					}
					else if (menu_select_key_pressed()) {
						menu_action_select();
					}
					else if (menu_delete_key_pressed()) {
						menu_action_delete();
					}
				}


				action_if_ped_assign_shortcut_key_pressed();

				action_if_ped_execute_shortcut_key_pressed();

				action_if_animation_sequence_shortcut_key_pressed(false);

				check_if_player_is_passenger_and_has_waypoint();

				check_if_actor_as_passenger_of_other_actor_key_pressed();

				check_if_explode_or_outofcontrol_nearby_vehicle_key_pressed();

				mainTickLast = GetTickCount();
			}

			/* ACTIONS WHICH ARE PERFORMED EVERY TICK */

			//Display HUD for app
			if (should_display_app_hud()) {
				draw_instructional_buttons();
				draw_menu();
				//disable ALT key
				CONTROLS::DISABLE_CONTROL_ACTION(0, 19, 1);
			}
			else {
				CONTROLS::ENABLE_CONTROL_ACTION(0, 19, 1);
			}

			draw_spot_lights();



			if (is_timlapse_active && GetTickCount() - timelapseLastTick > timelapseDeltaTicks) {
				action_timelapse_tick();
			}

			//check if the player is dead/arrested, in order to swap back to original in order to avoid crash
			check_player_model();

			//check if any recordings should be played
			for (auto & actor : actors) {
				if (actor.isNullActor() == false && actor.isCurrentlyPlayingRecording()) {
					Actor::update_tick_recording_replay(actor);
				}
			}

			//Wait for next tick
			WAIT(0);
		}else if (activeMode == MODE_BIRDS_EYE_VIEW) {

			bool exitMode = birdsEyeController.actionOnTick(GetTickCount(), actors, sceneStageLights);
			if (exitMode) {
				log_to_file("Exiting birds eye mode");
				activeMode = MODE_STANDARD;
				birdsEyeController.onExitMode();
			}

			if (hud_toggle_key_pressed()) {
				if (should_display_hud == true) {
					should_display_hud = false;
					WAIT(70);
				}
				else {
					should_display_hud = true;
					WAIT(70);
				}
			}


			//Wait for next tick
			WAIT(0);
		}
	}

}




void ScriptMain()
{
	srand(time(NULL));
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("CommonMenu", 0);

	scaleForm = GRAPHICS::REQUEST_SCALEFORM_MOVIE_INSTANCE("instructional_buttons");
	log_to_file("Waiting for instructional_buttons to load");
	while (!GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		WAIT(0);
	}
	log_to_file("instructional_buttons have loaded");

	action_show_info_on_start();

	init_read_keys_from_ini();

	gtaScenarios = getAllGTAScenarios();
	gtaWeatherTypes = getAllGTAWeather();
	modRelationshipGroups = getAllRelationshipGroups();
	gtaWalkingStyles = getAllMovementClipSet();
	for (auto & walkingStyle : gtaWalkingStyles) {
		STREAMING::REQUEST_CLIP_SET(walkingStyle.id);
	}

	bool animOk = initAnimations("SceneDirectorAnim.txt");
	if (animOk) {
		log_to_file("GTA Animations initialized: ");
	}
	else {
		log_to_file("GTA Animations initialization failed. SceneDirectorAnim.txt file missing?");
	}

	initializeSyncedAnimations();
	modSyncedAnimCategories = getAllSyncedAnimationCategories();


	create_relationship_groups();
	log_to_file("Screen Director initialized");

	//make sure police and everyone do not ignore player
	reset_ignore_player();

	main();
}