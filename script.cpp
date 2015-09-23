#include "script.h"
#include "keyboard.h"
#include "utils.h"
#include "scenario.h"
#include "weather.h"
#include "clipset_movement.h"
#include "lighting.h"

#include <string>
#include <ctime>
#include <vector>
#include <fstream>

//Key attributes
//These are overwrittein in init_read_keys_from_ini()
DWORD key_hud = VK_F10;
DWORD key_menu_up = VK_NUMPAD8;
DWORD key_menu_down = VK_NUMPAD2;
DWORD key_menu_left = VK_NUMPAD4;
DWORD key_menu_right = VK_NUMPAD6;
DWORD key_menu_select = VK_NUMPAD5;
char key_hud_str[256];

enum SCENE_MODE {
	SCENE_MODE_ACTIVE = 1,
	SCENE_MODE_SETUP = 0
};

enum MENU_ITEM {
	MENU_ITEM_ACTOR_1 = 1,
	MENU_ITEM_ACTOR_2 = 2,
	MENU_ITEM_ACTOR_3 = 3,
	MENU_ITEM_ACTOR_4 = 4,
	MENU_ITEM_ACTOR_5 = 5,
	MENU_ITEM_ACTOR_6 = 6,
	MENU_ITEM_ACTOR_7 = 7,
	MENU_ITEM_ACTOR_8 = 8,
	MENU_ITEM_ACTOR_9 = 9,
	MENU_ITEM_SCENE_MODE = 10,
	MENU_ITEM_AUTOPILOT = 11,
	MENU_ITEM_ESCORT = 12,
	MENU_ITEM_CHASE = 13,
	MENU_ITEM_FIRING_SQUAD = 14,
	MENU_ITEM_ADD_TO_SLOT = 15,
	MENU_ITEM_ADD_CLONE_TO_SLOT = 16,
	MENU_ITEM_CLONE = 17,
	MENU_ITEM_CLONE_WITH_VEHICLE = 18,
	MENU_ITEM_POSSESS = 19,
	MENU_ITEM_WORLD = 20,
	SUBMENU_ITEM_RECORD_PLAYER = 40,
	SUBMENU_ITEM_REMOVE_FROM_SLOT = 41,
	SUBMENU_ITEM_SPOT_LIGHT = 42,
	SUBMENU_ITEM_SPOT_LIGHT_COLOR = 43,
	SUBMENU_ITEM_DRUNK = 44,
	SUBMENU_ITEM_BLACKOUT = 50,
	SUBMENU_ITEM_TIMELAPSE = 51,
	SUBMENU_ITEM_WEATHER = 52,
	SUBMENU_ITEM_WIND = 53,


};

bool should_display_hud = false;

//attributes to the actors in the slot 1-9 
//index 0 is reserved for the last actor
Ped actorShortcut[10] = {};
bool actor0IsClone = false;
//waypoint for the acto
bool actorHasWaypoint[10] = {};
Vector3 actorWaypoint[10] = {};
//location when scene was set to active last time
bool actorHasStartLocation[10] = {};
Vector3 actorStartLocation[10] = {};
float actorStartLocationHeading[10] = {};
float actorDriverAgressiveness[] = { 0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f };
bool actorHasSpotlight[10] = {};
SPOT_LIGHT_TYPE actorSpotlightType[10] = {};
SpotLightColor actorSpotlightColor[10] = {};
bool actorHasWalkingStyle[10] = {};
ClipSet actorWalkingStyle[10] = {};

int blipIdShortcuts[10] = {};

bool is_autopilot_engaged_for_player = false;
bool is_chase_player_engaged = false;
int chase_player_index = -1;
bool is_escort_player_engaged = false;
int escort_player_index = -1;

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

//Represents the scene status of each actor
//1=Act on instructions immediately
//0=No action (scene setup mode)
//x=Not used (yet)
SCENE_MODE actorStatus[10] = { SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP,SCENE_MODE_SETUP };


std::vector<Scenario> gtaScenarios;

//used in passenger waypoint
int lastWaypointID = -1;

//used for world -> blackout
bool is_world_blackout = false;
bool is_timlapse_active = false;
int timelapse_delta_seconds = 0;
int timelapse_delta_minutes = 2;
DWORD timelapseLastTick = 0;
DWORD timelapseDeltaTicks = 1000;

std::vector<Weather> gtaWeatherTypes;
int index_weather = -1;

bool is_wind_active = false;

std::vector<ClipSet> gtaWalkingStyles;
int index_walking_style = -1;

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

//Used for determining wait time after actions
DWORD mainTickLast=0;
DWORD nextWaitTicks = 0;

int forceSlotIndexOverWrite = -1;


enum RECORDING_TYPE {
	RECORDING_TYPE_LOCATION,
	RECORDING_TYPE_EQUIP_WEAPON,
	RECORDING_TYPE_ENTER_VEHICLE,
};


class ActorRecordingItem {
	Vector3 location;
	float heading;
public:
	ActorRecordingItem(Vector3 aLocation, float aHeading) {
		location = aLocation;
		heading = aHeading;
	}
	Vector3 getLocation() {
		return location;
	}
	float getHeading() {
		return heading;
	}
	void addToTaskSequence(Ped ped) {
		AI::TASK_GO_STRAIGHT_TO_COORD(ped, location.x, location.y, location.z, 1.0f, -1, 27.0f, 0.5f);
	}
	std::string to_string() {
		return "ActorRecordingItem x:" + std::to_string(location.x) + " y : " + std::to_string(location.y) + " z : " + std::to_string(location.z) + " heading:" + std::to_string(heading);
	}
};

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

void log_to_file(std::string message, bool bAppend = true) {
	if (1) {
		std::ofstream logfile;
		char* filename = "scene_director.log";
		if (bAppend)
			logfile.open(filename, std::ios_base::app);
		else
			logfile.open(filename);
		logfile << currentDateTime() << " " << message + "\n";
		logfile.close();
	}
	return;
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



// player model control, switching on normal ped model when needed	
void check_player_model()
{
	// common variables
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

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
		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			//log_to_file("store_current_waypoint_for_actor: Ped id:" + std::to_string(actorShortcut[i]) );
			if (actorShortcut[i] == ped) {
				log_to_file("store_current_waypoint_for_actor:Found match");
				actorWaypoint[i] = waypointCoord;
				actorHasWaypoint[i] = true;
				return;
			}
		}
		log_to_file("store_current_waypoint_for_actor: Found no match");
	}

}

int get_index_for_actor(Ped ped) {
	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i] == ped) {
			return i;
		}
	}
	return -1;
}

int get_next_free_slot() {
	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i] == 0) {
			return i;
		}
	}
	return -1;
}


bool is_ped_actor_active(Ped ped) {
	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i] == ped) {
			if (actorStatus[i] == SCENE_MODE_SETUP) {
				return false;
			}
			else {
				return true;
			}
		}
	}
	//if ped is not in a slot, return the status of the overall scene
	if (sceneMode == SCENE_MODE_SETUP) {
		return false;
	}
	else {
		return true;
	}

}


bool should_display_app_hud() {
	return should_display_hud;
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

	PED::ADD_RELATIONSHIP_GROUP("ACTOR1_GROUP", actorHashGroupP);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, actorHashGroup, GAMEPLAY::GET_HASH_KEY("player"));
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, actorHashGroup, actorHashGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GAMEPLAY::GET_HASH_KEY("player"), actorHashGroup);

	PED::ADD_RELATIONSHIP_GROUP("ACTOR1_GROUP", actorHashGroupP);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, actorHashGroup, GAMEPLAY::GET_HASH_KEY("player"));
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, actorHashGroup, actorHashGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GAMEPLAY::GET_HASH_KEY("player"), actorHashGroup);

}

void assign_actor_to_relationship_group(Ped ped) {
	log_to_file("Adding actor to relationship group . Existing hash is " +std::to_string(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped))+ " actorHashGroup is " + std::to_string(actorHashGroup));
	
	 
	if(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped) == actorHashGroup) {
		log_to_file("Ped already belongs to actor relationship group");
	}else {
		PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, actorHashGroup);
	}
	log_to_file("Relationship group after add " + std::to_string(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped)) + " actorHashGroup is " + std::to_string(actorHashGroup));
}

void draw_instructional_buttons() {
	if (GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		//_instructionalButtonsScaleform.CallFunction("CLEAR_ALL");
		GRAPHICS::_CALL_SCALEFORM_MOVIE_FUNCTION_VOID(scaleForm, "CLEAR_ALL");

		//_instructionalButtonsScaleform.CallFunction("TOGGLE_MOUSE_BUTTONS", 0);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "TOGGLE_MOUSE_BUTTONS");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_BOOL(0);
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		//_instructionalButtonsScaleform.CallFunction("CREATE_CONTAINER");
		//not used in scripts, not sure if required
		GRAPHICS::_CALL_SCALEFORM_MOVIE_FUNCTION_VOID(scaleForm, "CREATE_CONTAINER");

		//_instructionalButtonsScaleform.CallFunction("SET_DATA_SLOT", 0, Function.Call<string>(Hash._0x0499D7B09FC9B407, 2, (int)Control.PhoneSelect, 0), "Select");

		char* altControlKey = CONTROLS::_0x0499D7B09FC9B407(2, 19, 1);
		char* spaceControlKey = CONTROLS::_0x0499D7B09FC9B407(2, 22, 1);
		char* delControlKey = CONTROLS::_0x0499D7B09FC9B407(2, 178, 1);
		char* insControlKey = CONTROLS::_0x0499D7B09FC9B407(2, 121, 1);

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(0);
		std::string scaleFormKey = "t_" + std::string(key_hud_str);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING(strdup(scaleFormKey.c_str()));
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Hide HUD menu");
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

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(2);
		GRAPHICS::_0xE83A3E3557A56640(insControlKey);
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Waypoint to all");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(3);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_A");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Autopilot");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();


		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(4);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("t_C");
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Firing squad");
		GRAPHICS::_POP_SCALEFORM_MOVIE_FUNCTION_VOID();

		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION(scaleForm, "SET_DATA_SLOT");
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_INT(5);
		GRAPHICS::_0xE83A3E3557A56640(delControlKey);
		GRAPHICS::_0xE83A3E3557A56640(altControlKey);
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_FUNCTION_PARAMETER_STRING("Reset scene");
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
		GRAPHICS::_PUSH_SCALEFORM_MOVIE_RGBA(scaleForm, 255, 255, 255, 255);
	}
	else {
		log_to_file("Scaleform has not loaded. scaleForm has value " + std::to_string(scaleForm));
	}
}

void draw_submenu_world(int drawIndex) {
	int submenu_index = 0;

	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
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

void draw_submenu_player(int drawIndex) {
	int submenu_index = 0;
	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());

	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB = 255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_RECORD_PLAYER;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	DRAW_TEXT("Start recording", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


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
		submenu_active_action = SUBMENU_ITEM_SPOT_LIGHT;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	if (actorIndex != -1 && actorHasSpotlight[actorIndex] == true) {
		std::string spotLightText ="Spot light: " + getNameForSpotLightType(actorSpotlightType[actorIndex]);
		DRAW_TEXT(strdup(spotLightText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	else {
		DRAW_TEXT("Spot light: None", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);


	if (actorIndex != -1 && actorHasSpotlight[actorIndex] == true) {
		drawIndex++;
		submenu_index++;
		if (submenu_is_active && submenu_active_index == submenu_index) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
			submenu_active_action = SUBMENU_ITEM_SPOT_LIGHT_COLOR;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}

		std::string spotLightColorText = "Spot color: " + actorSpotlightColor[actorIndex].name;
		DRAW_TEXT(strdup(spotLightColorText.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	}


	drawIndex++;
	submenu_index++;
	if (submenu_is_active && submenu_active_index == submenu_index) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		submenu_active_action = SUBMENU_ITEM_DRUNK;
	}
	else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}




	if (actorIndex != -1 && actorHasWalkingStyle[actorIndex] == true) {
		std::string walkingStyleString = "Walk: " + std::string(actorWalkingStyle[actorIndex].name);
		DRAW_TEXT(strdup(walkingStyleString.c_str()), 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	else {
		DRAW_TEXT("Walk: Normal", 0.76, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	}
	GRAPHICS::DRAW_RECT(0.81, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

	submenu_max_index = submenu_index;
}


void draw_menu() {
	int drawIndex = 0;
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	int actorIndexPlayer = get_index_for_actor(playerPed);


	//colors for swapping from active to inactive... messy
	int textColorR = 255, textColorG = 255, textColorB=255;
	int bgColorR = 0, bgColorG = 0, bgColorB = 0;
	if (menu_active_index == drawIndex) {
		textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
	} else {
		textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
	}


	//1. If actor is not assigned to any slot
	if (actorIndexPlayer == -1) {
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

	//4. World options
	if (menu_active_index == drawIndex) {
		menu_active_action = MENU_ITEM_SCENE_MODE;
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
	DRAW_TEXT("World options", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
	GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

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

	//5. If chase is engaged
	if (is_chase_player_engaged) {
		DRAW_TEXT("Player chase: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_CHASE;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	} else if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		DRAW_TEXT("Activate chase", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_CHASE;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	}


	//6. If escort is engaged
	if (is_escort_player_engaged) {
		DRAW_TEXT("Player escort: Active", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_ESCORT;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	} else if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
		DRAW_TEXT("Activate escort", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
		GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);
		if (menu_active_index == drawIndex) {
			menu_active_action = MENU_ITEM_ESCORT;
		}

		drawIndex++;
		if (menu_active_index == drawIndex) {
			textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
		}
		else {
			textColorR = 255, textColorG = 255, textColorB = 255, bgColorR = 0, bgColorG = 0, bgColorB = 0;
		}
	}

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
	DRAW_TEXT("Possess nearest/aimed", 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
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
	for (int i = (sizeof(actorShortcut) / sizeof(Ped))-1 ; i >0 ; i--) {
		if (actorShortcut[i] != 0) {
			//check if we should move the selected index to this actor ( menu_active_ped is set when switching to an actor)
			if (menu_active_index == -1 && actorIndexPlayer == i) {
				textColorR = 0, textColorG = 0, textColorB = 0, bgColorR = 255, bgColorG = 255, bgColorB = 255;
				menu_active_index = drawIndex;
				menu_active_ped = 0;
			}

			//show submenu for active player
			if (menu_active_index == drawIndex && actorIndexPlayer == i) {
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

			char* actorText = strdup(("Actor "+ std::to_string(i)).c_str());
			DRAW_TEXT(actorText, 0.88, 0.888 - (0.04)*drawIndex, 0.3, 0.3, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			GRAPHICS::DRAW_RECT(0.93, 0.900 - (0.04)*drawIndex, 0.113, 0.034, bgColorR, bgColorG, bgColorB, 100);

			/* Can't find icon for waypoint, so just using text for now
			if (GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED("CommonMenu") && actorHasWaypoint[i]) {
				GRAPHICS::DRAW_SPRITE("CommonMenu", "MP_AlertTriangle", 0.95, 0.888 - (0.04)*drawIndex, 0.08, 0.08, 0, 255, 255, 255, 50);
			}*/

			if (actorHasWaypoint[i] != 0) {
				DRAW_TEXT("Waypoint", 0.959, 0.885 - (0.04)*drawIndex, 0.18, 0.18, 0, false, false, false, false, textColorR, textColorG, textColorB, 200);
			}

			if (actorIndexPlayer == i) {
				GRAPHICS::DRAW_RECT(0.93, 0.883 - (0.04)*drawIndex, 0.113, 0.002, 100, 255, 0, 100);				
			}


			if (menu_active_index == drawIndex) {
				//i should match value of MENU_ITEM_ACTOR_X
				menu_active_action = (MENU_ITEM)i;
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
	//draw spotlights
	for (int i = 1; i < sizeof(actorHasSpotlight) / sizeof(bool); i++) {
		if (actorHasSpotlight[i]) {
			//log_to_file("Drawing spot light for actor with index " + std::to_string(i));
			Vector3 actorPos = ENTITY::GET_ENTITY_COORDS(actorShortcut[i], true);
			int colorR = actorSpotlightColor[i].r;
			int colorG = actorSpotlightColor[i].g;
			int colorB = actorSpotlightColor[i].b;

			
			switch (actorSpotlightType[i]) {
			case SPOT_LIGHT_NONE:
				break;
			case SPOT_LIGHT_ACTOR_ABOVE:
				GRAPHICS::DRAW_SPOT_LIGHT(actorPos.x, actorPos.y, actorPos.z + 20.0, 0, 0, -1.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 4.0f, 1.0f);
				break;
			case SPOT_LIGHT_ACTOR_LEFT:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x+10.0, actorPos.y, actorPos.z, -1.0, 0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			case SPOT_LIGHT_ACTOR_RIGHT:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x-10.0, actorPos.y, actorPos.z, 1.0, 0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			case SPOT_LIGHT_ACTOR_INFRONT:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x, actorPos.y - 10.0, actorPos.z, 0.0, 1.0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			case SPOT_LIGHT_ACTOR_BEHIND:
				GRAPHICS::_DRAW_SPOT_LIGHT_WITH_SHADOW(actorPos.x, actorPos.y+10.0, actorPos.z, 0.0, -1.0, 0.0, colorR, colorG, colorB, 100.0f, 1.0, 0.0f, 6.0f, 1.0f,0);
				break;
			default:
				break;
			}

			
		}
	}
}


void move_to_waypoint(Ped ped, Vector3 waypointCoord, bool suppress_msgs) {
	log_to_file("move_to_waypoint: Ped:" + std::to_string(ped) + " x:" + std::to_string(waypointCoord.x) + " y : " + std::to_string(waypointCoord.y) + " z : " + std::to_string(waypointCoord.z));
	//code inspired by LUA plugin https://www.gta5-mods.com/scripts/realistic-vehicle-controls
	if (is_ped_actor_active(ped)) {


		if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);

			//check if player is the driver
			Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
			if (pedDriver != ped) {
				log_to_file("move_to_waypoint: Ped (" + std::to_string(ped)+ " is not driver (" + std::to_string(pedDriver));
				//set_status_text("Ped is not driver. Ignore waypoint");
			}
			else {
				int actorIndex = get_index_for_actor(pedDriver);
				if (actorIndex != -1) {
					actorHasWaypoint[actorIndex] = true;
					actorWaypoint[actorIndex] = waypointCoord;
				}

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
					AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, vehicleMaxSpeed, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786469,5.0, -1);
					log_to_file("move_to_waypoint: Driving with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));
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

		}
		else if (PED::IS_PED_ON_FOOT(ped)) {
			AI::TASK_GO_STRAIGHT_TO_COORD(ped, waypointCoord.x, waypointCoord.y, waypointCoord.z, 1.0f, -1, 27.0f, 0.5f);
			log_to_file("move_to_waypoint: Ped (" + std::to_string(ped) + " is walking to waypoint");
			if (suppress_msgs != true) {
				set_status_text("Walking to waypoint");
			}

		}
	}

}

void playback_recording_to_waypoint(Ped ped, Vector3 waypointCoord) {
	log_to_file("playback_recording_to_waypoint: Ped:" + std::to_string(ped) + " x:" + std::to_string(waypointCoord.x) + " y : " + std::to_string(waypointCoord.y) + " z : " + std::to_string(waypointCoord.z));

	if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {
		Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);

		//check if player is the driver
		Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
		if (pedDriver != ped) {
			log_to_file("move_to_waypoint: Ped (" + std::to_string(ped) + " is not driver (" + std::to_string(pedDriver));
			//set_status_text("Ped is not driver. Ignore waypoint");
		}
		else {
			int actorIndex = get_index_for_actor(pedDriver);
			if (actorIndex != -1) {
				actorHasWaypoint[actorIndex] = true;
				actorWaypoint[actorIndex] = waypointCoord;
			}

			float vehicleMaxSpeed = VEHICLE::_GET_VEHICLE_MAX_SPEED(ENTITY::GET_ENTITY_MODEL(pedVehicle));

			if (PED::IS_PED_IN_ANY_HELI(ped)) {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, vehicleMaxSpeed, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
				log_to_file("move_to_waypoint: Flying in heli with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed) );
			}
			else if (PED::IS_PED_IN_ANY_PLANE(ped)) {
				AI::TASK_PLANE_MISSION(pedDriver, pedVehicle, 0, 0, waypointCoord.x, waypointCoord.y, waypointCoord.z, 4, 30.0, 50.0, -1, vehicleMaxSpeed, 50);
				log_to_file("move_to_waypoint: Flying in plane with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed) );

			}
			else if (PED::IS_PED_IN_ANY_BOAT(ped)) {
				AI::TASK_BOAT_MISSION(pedDriver, pedVehicle, 0, 0, waypointCoord.x, waypointCoord.y, waypointCoord.z, 4, vehicleMaxSpeed, 786469, -1.0, 7);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(pedDriver, 1);
				//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, 0.0, 20.0, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786469, 5.0, 1071);
				log_to_file("move_to_waypoint: In boat : " + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));
			}
			else {
				AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, vehicleMaxSpeed, 0, ENTITY::GET_ENTITY_MODEL(pedVehicle), 786469, 5.0, -1);
				log_to_file("move_to_waypoint: Driving with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));

			}
		}

	}
	else if (PED::IS_PED_ON_FOOT(ped)) {
		AI::TASK_GO_STRAIGHT_TO_COORD(ped, waypointCoord.x, waypointCoord.y, waypointCoord.z, 1.0f, -1, 27.0f, 0.5f);
		log_to_file("move_to_waypoint: Ped (" + std::to_string(ped) + " is walking to waypoint");
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
			if (GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, groundCheckHeight[i], &location.z))
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
		VEHICLE::SET_VEHICLE_ENGINE_ON(entityToTeleport, false, true);
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
		int actor_index = get_index_for_actor(swapFromPed);
		if (actor_index != -1 && actorHasWaypoint[actor_index]) {
			move_to_waypoint(swapFromPed, actorWaypoint[actor_index],true);
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

		actorShortcut[0] = swapFromPed;
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
		PLAYER::_GET_AIMED_ENTITY(player, &targetEntity);

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
	actorShortcut[0] = clonedPed;
	actor0IsClone = true;

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		PED::SET_PED_INTO_VEHICLE(clonedPed, vehicle, -2);
	}
	else {//clone a bit ahead of the player and facing towards you
		ENTITY::SET_ENTITY_HEADING(clonedPed, ENTITY::GET_ENTITY_HEADING(playerPed)+180.0);
		teleport_entity_to_location(clonedPed, ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, 1.0, 3.0, 0.0), false);
	}

	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(clonedPed, true);
	assign_actor_to_relationship_group(clonedPed);

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

		actorShortcut[0] = clonedPed;
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

	AI::TASK_ENTER_VEHICLE(playerPed, vehicle, -1, 0, 1.0, 1, 0);
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

						int actorIndex = get_index_for_actor(pedDriver);
						if (actorIndex != -1) {
							actorHasWaypoint[actorIndex] = true;
							actorWaypoint[actorIndex] = waypointCoord;
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


}

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


bool swap_to_previous_possessed_key_pressed()
{
	//see https://msdn.microsoft.com/de-de/library/windows/desktop/dd375731(v=vs.85).aspx for key codes	
	return IsKeyDown(VK_CONTROL) && IsKeyDown(0x30);
}

void add_ped_to_slot(int slotIndex, Ped ped) {
	//check if there exist an actor for this index
	if (actorShortcut[slotIndex] != 0) {
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
			int blipIdsToRemove[1] = { blipIdShortcuts[slotIndex] };
			UI::REMOVE_BLIP(blipIdsToRemove);
			//and continue storing the actor
		}

	}

	actorShortcut[slotIndex] = ped;

	assign_actor_to_relationship_group(ped);

	int blipId = UI::ADD_BLIP_FOR_ENTITY(ped);
	blipIdShortcuts[slotIndex] = blipId;
	//BLIP Sprite for nr1=17, nr9=25
	UI::SET_BLIP_SPRITE(blipId, 16 + slotIndex);

	//Store current waypoint
	if (UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		actorWaypoint[slotIndex] = waypointCoord;
		actorHasWaypoint[slotIndex] = true;
	}

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
	if (actorShortcut[pedShortcutsIndex] == 0) {
		set_status_text("No stored actor in slot " + std::to_string(pedShortcutsIndex));
	}
	else {
		actorShortcut[pedShortcutsIndex] = 0;
		actorHasWaypoint[pedShortcutsIndex] = false;
		actorWaypoint[pedShortcutsIndex] = Vector3();
		actorHasStartLocation[pedShortcutsIndex] = false;
		actorStartLocation[pedShortcutsIndex] = Vector3();
		actorStartLocationHeading[pedShortcutsIndex] = 0.0;
		//remove blip
		int blipIdsToRemove[1] = { blipIdShortcuts[pedShortcutsIndex] };
		UI::REMOVE_BLIP(blipIdsToRemove);
		blipIdShortcuts[pedShortcutsIndex] = 0;
	}
}

void swap_to_actor_with_index(int pedShortcutsIndex) {
	if (actorShortcut[pedShortcutsIndex] == 0) {
		set_status_text("No stored actor. Store with CTRL+" + std::to_string(pedShortcutsIndex));
	}
	else {
		log_to_file("action_if_ped_execute_shortcut_key_pressed: Retrieve ped in slot " + std::to_string(pedShortcutsIndex));
		Ped pedInSlot = actorShortcut[pedShortcutsIndex];
		if (ENTITY::IS_ENTITY_DEAD(pedInSlot)) {
			log_to_file("action_if_ped_execute_shortcut_key_pressed: Dead ped in slot " + std::to_string(pedShortcutsIndex));
			set_status_text("Thou shalt not swap to a dead actor");
		}
		else {
			log_to_file("action_if_ped_execute_shortcut_key_pressed: Switching to ped:  " + std::to_string(actorShortcut[pedShortcutsIndex]));

			//first store the waypoint for the current actor
			store_current_waypoint_for_actor(PLAYER::PLAYER_PED_ID());

			possess_ped(actorShortcut[pedShortcutsIndex]);
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

			swap_to_actor_with_index(pedShortcutsIndex);
		}
	}
}



void action_reset_scene_director() {
	set_status_text("Resetting scene director to initial status");

	UI::REMOVE_BLIP(blipIdShortcuts);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
			Ped actor = actorShortcut[i];
			if (PED::IS_PED_IN_ANY_VEHICLE(actor, 0)) {
				Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_USING(actor);
				ENTITY::SET_ENTITY_AS_MISSION_ENTITY(playerVehicle, false, false);
				ENTITY::SET_VEHICLE_AS_NO_LONGER_NEEDED(&playerVehicle);
			}
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(actor, false, false);
			ENTITY::SET_PED_AS_NO_LONGER_NEEDED(&actor);
		}
		actorHasWaypoint[i] = false;
		actorWaypoint[i].x = 0;
		actorWaypoint[i].y = 0;
		actorWaypoint[i].z = 0;
		actorStartLocation[i].x = 0;
		actorStartLocation[i].y = 0;
		actorStartLocation[i].z = 0;
		actorHasStartLocation[i] = false;
		if (blipIdShortcuts[i] != 0) {
			int blipIdsToRemove[1] = { blipIdShortcuts[i] };
			UI::REMOVE_BLIP(blipIdsToRemove);
			blipIdShortcuts[i] = 0;
		}

	}
}

void action_vehicle_chase() {
	log_to_file("action_vehicle_chase");
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (is_chase_player_engaged) {
		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
				AI::TASK_PAUSE(actorShortcut[i], 500);
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

		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
				ensure_ped_and_vehicle_is_not_deleted(actorShortcut[i]);

				Vehicle actorVehicle = PED::GET_VEHICLE_PED_IS_USING(actorShortcut[i]);

				//chase is based on type of vehicles for both player and actor
				if (PED::IS_PED_IN_ANY_HELI(playerPed) && PED::IS_PED_IN_ANY_HELI(actorShortcut[i])) {
					//both are in heli
					log_to_file("TASK_HELI_CHASE1 - Actor " + std::to_string(actorShortcut[i]));
					//last three params are distance to playerVehicle
					AI::TASK_HELI_CHASE(actorShortcut[i], playerVehicle, 0.0, 0.0, -5.0);
				}
				else if (PED::IS_PED_IN_ANY_HELI(actorShortcut[i])) {
					//if only actor (not player) is in heli
					log_to_file("TASK_HELI_CHASE2 - Actor " + std::to_string(actorShortcut[i]));
					AI::TASK_HELI_CHASE(actorShortcut[i], playerVehicle, 0.0, 0.0, -5.0);
				}
				else if (PED::IS_PED_IN_ANY_PLANE(playerPed) && PED::IS_PED_IN_ANY_PLANE(actorShortcut[i])) {
					//both are in a plane
					log_to_file("TASK_PLANE_CHASE1 - Actor " + std::to_string(actorShortcut[i]));
					AI::TASK_PLANE_CHASE(actorShortcut[i], playerVehicle, 0.0, -5.0, -5.0);
				}
				else if (PED::IS_PED_IN_ANY_PLANE(actorShortcut[i])) {
					//if only actor (not player) is in plane
					log_to_file("TASK_PLANE_CHASE2 - Actor " + std::to_string(actorShortcut[i]));
					AI::TASK_PLANE_CHASE(actorShortcut[i], playerVehicle, 0.0, 0.0, 5.0);
				}
				else {//standard vehicle chase
					log_to_file("TASK_VEHICLE_CHASE - Actor " + std::to_string(actorShortcut[i]));
					AI::TASK_VEHICLE_CHASE(actorShortcut[i], playerPed);
				}
			}
		}
		set_status_text("Vehicle chase has started");
		is_chase_player_engaged = true;
		chase_player_index = get_index_for_actor(playerPed);

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
		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
				AI::TASK_PAUSE(actorShortcut[i], 500);
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
		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
				ensure_ped_and_vehicle_is_not_deleted(actorShortcut[i]);

				if (PED::IS_PED_IN_ANY_VEHICLE(actorShortcut[i], 0)) {

					Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorShortcut[i]);

					//check if player is a passenger
					Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);

					if (pedDriver != playerPed) {
						//AI::TASK_VEHICLE_ESCORT(pedDriver, pedVehicle, playerVehicle, -1, VEHICLE::_GET_VEHICLE_MAX_SPEED(pedVehicle), 786469, 10.0, -1, 10.0);

						//works
						//AI::TASK_VEHICLE_ESCORT(pedDriver, pedVehicle, playerVehicle, -1, 13.0, 786603, 8.0, 20, 5.0);
						log_to_file("action_vehicle_escort:AI::TASK_VEHICLE_ESCORT Driver: " + std::to_string(pedDriver));
						AI::TASK_VEHICLE_ESCORT(pedDriver, pedVehicle, playerVehicle, -1, 45.0, 786469, 8.0, 20, 5.0);

					}
				}

			}
		}

		is_escort_player_engaged = true;
		escort_player_index = get_index_for_actor(playerPed);

		set_status_text("Vehicle escort has started");
		nextWaitTicks = 300;
	}
	else {
		log_to_file("action_vehicle_escort not started. Player missing vehicle");
		set_status_text("You need a vehicle in order to start a vehicle escort");
	}
}

void action_autopilot_for_player() {
	log_to_file("action_autopilot_for_player");
	nextWaitTicks = 300;

	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	//update the waypoint if one is set currently
	if (actorIndex != -1 && UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		actorHasWaypoint[actorIndex] = true;
		actorWaypoint[actorIndex] = waypointCoord;
	}

	if (actorIndex != -1 && actorHasWaypoint[actorIndex]) {
		if (PED::IS_PED_IN_ANY_VEHICLE(actorShortcut[actorIndex], 0)) {
			Vehicle pedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorShortcut[actorIndex]);

			//check if player is a passenger
			Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(pedVehicle, -1);
			log_to_file("action_autopilot_for_player " + std::to_string(pedDriver) +" vs " + std::to_string(actorShortcut[actorIndex]) + " vehicle " + std::to_string(pedVehicle));
			if (pedDriver == actorShortcut[actorIndex]) {
				move_to_waypoint(actorShortcut[actorIndex], actorWaypoint[actorIndex],true);
				log_to_file("Autopilot engaged for player " + std::to_string(actorIndex));
				set_status_text("Autopilot engaged for player");

				is_autopilot_engaged_for_player = true;
			}
		}
	} else {
		set_status_text("Actor must be assigned a slot 1-9 before autopilot can be started");
	}
}


void action_set_same_waypoint_for_all_actors() {
	if (UI::IS_WAYPOINT_ACTIVE()) {
		int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
		log_to_file("action_set_same_waypoint_for_all_actors x:" + std::to_string(waypointCoord.x)+ " y:" + std::to_string(waypointCoord.y) + " z:" + std::to_string(waypointCoord.z));

		set_status_text("Waypoint set for all actors");

		//add waypoint to all actors in slots
		//But ignore ALT+0 as this a duplicate
		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			actorStatus[i] = sceneMode;

			//if not actor in slot, continue
			if (actorShortcut[i] == 0) {
				continue;
			}
			actorWaypoint[i] = waypointCoord;
			actorHasWaypoint[i] = true;

			//act on the waypoint (will not do anything if SCENE_MODE_SETUP

			//first if he's a driver
			move_to_waypoint(actorShortcut[i], actorWaypoint[i], true);
			//second if he's a passenger
			check_if_ped_is_passenger_and_has_waypoint(actorShortcut[i]);
			WAIT(200);

		}

	}
	else {
		set_status_text("Set a waypoint in the map, before applying it to all actors");
	}
}

void action_teleport_to_start_locations() {
	set_status_text("Resetting the scene. Will take a few seconds");
	log_to_file("action_teleport_to_start_locations");
	for (int i = 0; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i]!= 0 && actorHasStartLocation[i]) {
			Ped entityToTeleport = actorShortcut[i];

			//ressurect any dead actors
			if (ENTITY::IS_ENTITY_DEAD(actorShortcut[i])) {
				PED::RESURRECT_PED(actorShortcut[i]);
			}

			//pause the actor. seems to be stuck some times


			//AI::CLEAR_PED_TASKS(actorShortcut[i]);
			//AI::TASK_PAUSE(actorShortcut[i], 10000);

			if (PED::IS_PED_IN_ANY_VEHICLE(entityToTeleport, 0)) {
				entityToTeleport = PED::GET_VEHICLE_PED_IS_USING(entityToTeleport);
				VEHICLE::SET_VEHICLE_ENGINE_ON(entityToTeleport, false, true);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(entityToTeleport, true);

			}

			//try to prevent fleeing during teleport
			AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actorShortcut[i], true);
			PED::SET_PED_FLEE_ATTRIBUTES(actorShortcut[i], 0, 0);

			//teleport and wait
			teleport_entity_to_location(entityToTeleport, actorStartLocation[i], true);
			ENTITY::SET_ENTITY_HEADING(entityToTeleport, actorStartLocationHeading[i]);

			WAIT(300);
			//set back this block


		}

	}
	log_to_file("Setting vehicle to undrivable");
	for (int i = 0; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i] != 0 && actorHasStartLocation[i]) {

			if (PED::IS_PED_IN_ANY_VEHICLE(actorShortcut[i], 0)) {
				Vehicle teleportedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorShortcut[i]);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(teleportedVehicle, true);
				VEHICLE::SET_VEHICLE_ALARM(teleportedVehicle, true);
				VEHICLE::START_VEHICLE_ALARM(teleportedVehicle);
			}
		}
	}

	WAIT(1000);

	log_to_file("Setting vehicle to drivable");
	Ped orgPed = PLAYER::PLAYER_PED_ID();

	for (int i = 0; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		if (actorShortcut[i] != 0 && actorHasStartLocation[i] && actorShortcut[i] != orgPed) {
			possess_ped(actorShortcut[i]);
			WAIT(250);
			if (PED::IS_PED_IN_ANY_VEHICLE(actorShortcut[i], 0)) {

				Vehicle teleportedVehicle = PED::GET_VEHICLE_PED_IS_USING(actorShortcut[i]);
				VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(teleportedVehicle);
				VEHICLE::SET_VEHICLE_ALARM(teleportedVehicle, false);
				VEHICLE::SET_VEHICLE_UNDRIVEABLE(teleportedVehicle, false);
			}
		}
	}

	possess_ped(orgPed);
	if (PED::IS_PED_IN_ANY_VEHICLE(orgPed, 0)) {
		Vehicle teleportedVehicle = PED::GET_VEHICLE_PED_IS_USING(orgPed);
		VEHICLE::SET_VEHICLE_ALARM(teleportedVehicle, false);
		VEHICLE::SET_VEHICLE_UNDRIVEABLE(teleportedVehicle, false);
	}

}

void action_timelapse_tick() {
	TIME::ADD_TO_CLOCK_TIME(0, timelapse_delta_minutes, 0);
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
	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	index_walking_style++;
	if (index_walking_style > gtaWalkingStyles.size() - 1) {
		PED::RESET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), 0.0);
		actorHasWalkingStyle[actorIndex] = false;
		actorWalkingStyle[actorIndex] = ClipSet();
		index_walking_style = -1;
		set_status_text("Walking style is now back to normal");
	}
	else {
		ClipSet walkingStyle = gtaWalkingStyles[index_walking_style];
		if (STREAMING::HAS_CLIP_SET_LOADED(walkingStyle.id)) {
			PED::SET_PED_MOVEMENT_CLIPSET(PLAYER::PLAYER_PED_ID(), walkingStyle.id, 1.0);
			actorHasWalkingStyle[actorIndex] = true;
			actorWalkingStyle[actorIndex] = walkingStyle;
		}
		else {
			log_to_file("Clipset has not loaded yet");
		}
		
		//set_status_text("Weather is now: " + std::string(weather.id));
	}
}

void action_next_spot_light() {
	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	if(actorIndex != -1){
		if (actorHasSpotlight[actorIndex] == false) {
			log_to_file("Turning on spot light for index " + std::to_string(actorIndex));
			actorHasSpotlight[actorIndex] = true;
			actorSpotlightType[actorIndex] = SPOT_LIGHT_ACTOR_ABOVE;
			actorSpotlightColor[actorIndex] = getDefaultSpotLightColor();
		}
		else {
			if (actorSpotlightType[actorIndex] == last_spot_light_type) {
				actorHasSpotlight[actorIndex] = false;
				actorSpotlightType[actorIndex] = SPOT_LIGHT_NONE;
				log_to_file("Turning off spot light for index " + std::to_string(actorIndex));
			}
			else {
				int existing_spot_light_type = actorSpotlightType[actorIndex];
				actorSpotlightType[actorIndex] = (SPOT_LIGHT_TYPE) (++existing_spot_light_type);
			}	
		}
	}
}

void action_next_spot_light_color() {
	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	if (actorIndex != -1) {
		if (actorHasSpotlight[actorIndex]) {
			actorSpotlightColor[actorIndex] = getNextSpotLightColor(actorSpotlightColor[actorIndex]);
		}
	}
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
	}



	//trigger the action for all actors in slots
	//But ignore ALT+0 as this a duplicate
	for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
		actorStatus[i] = sceneMode;


		//if not actor in slot, continue
		if (actorShortcut[i] == 0) {
			continue;
		}

		ensure_ped_and_vehicle_is_not_deleted(actorShortcut[i]);

		if (actorStatus[i] == SCENE_MODE_ACTIVE) {
			//log_to_file("Actor " + std::to_string(i) + " Ped id:" + std::to_string(actorShortcut[i]) + " Has waypoint:"+ std::to_string(actorHasWaypoint[i])+  " Has start location:"+ std::to_string(actorHasStartLocation[i]));

			//store the current location of all actors, so that we can reset it
			actorStartLocation[i] = ENTITY::GET_ENTITY_COORDS(actorShortcut[i], true);
			actorStartLocationHeading[i] = ENTITY::GET_ENTITY_HEADING(actorShortcut[i]);
			actorHasStartLocation[i] = true;

			//move the actor if he has a waypoint and if he's not the player
			if (actorHasWaypoint[i] && actorShortcut[i] != PLAYER::PLAYER_PED_ID()) {
				//first if he's a driver
				move_to_waypoint(actorShortcut[i], actorWaypoint[i], true);
				//second if he's a passenger
				check_if_ped_is_passenger_and_has_waypoint(actorShortcut[i]);
				WAIT(200);
			}
		}
		else if (actorStatus[i] == SCENE_MODE_SETUP) {
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actorStatus[i], true);
			//AI::TASK_STAND_STILL(actorShortcut[i], -1);
			AI::CLEAR_PED_TASKS(actorShortcut[i]);
			//AI::TASK_PAUSE(actorShortcut[i], 500);
		}
	}
	nextWaitTicks = 500;
}




bool record_scene_for_actor_key_press() {
	//ALT+ R
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x52)) {
		return true;
	}
	else {
		return false;
	}
}

void action_record_scene_for_actor() {


	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	if (actorIndex == -1) {
		set_status_text("Actor must be assigned slot 1-9 before recording actions");
	}
	else {
		set_status_text("Recording actions for current actions. Press ALT+R to stop recording");
		Ped actorPed = actorShortcut[actorIndex];

		//the actual recording
		std::vector<ActorRecordingItem> actorRecording;
		actorRecording.reserve(1000);

		//1. Store start location
		actorStartLocation[actorIndex] = ENTITY::GET_ENTITY_COORDS(actorPed, true);
		actorStartLocationHeading[actorIndex] = ENTITY::GET_ENTITY_HEADING(actorPed);
		actorHasStartLocation[actorIndex] = true;

		WAIT(300);

		bool bRecording = true;
		DWORD tickStart = GetTickCount();
		DWORD tickLast = tickStart;
		DWORD tickNow = tickStart;
		CONST DWORD DELTA_TICKS = 1000;
		//4000 works well for boats

		//main loop
		while (bRecording == true) {
			tickNow = GetTickCount();

			//record only once pr DELTA_TICKS
			if (tickNow - tickLast >= DELTA_TICKS) {


				Vector3 actorLocation = ENTITY::GET_ENTITY_COORDS(actorPed, true);
				float actorHeading = ENTITY::GET_ENTITY_HEADING(actorPed);

				ActorRecordingItem recordingItem(actorLocation, actorHeading);
				actorRecording.push_back(recordingItem);

				log_to_file(recordingItem.to_string());


				tickLast = tickNow;
			}



			if (record_scene_for_actor_key_press()) {
				bRecording = false;
			}
			WAIT(0);
		}
		log_to_file("Recorded " + std::to_string(actorRecording.size()) + " instructions");

		//action_possess_ped();


		Entity entityToTeleport = actorPed;
		if (PED::IS_PED_IN_ANY_VEHICLE(entityToTeleport, 0)) {
			entityToTeleport = PED::GET_VEHICLE_PED_IS_USING(entityToTeleport);
		}

		teleport_entity_to_location(entityToTeleport, actorStartLocation[actorIndex], true);
		ENTITY::SET_ENTITY_HEADING(entityToTeleport, actorStartLocationHeading[actorIndex]);

		/* Attempt 1: AI::TASK_GO_STRAIGHT_TO_COORD  - Kind of works, but the actor pauses between each coord before moving to the next
		TaskSequence actorSeq;
		AI::OPEN_SEQUENCE_TASK(&actorSeq);
		//0xec17e58, 0xbac0f10b, 0x3f67c6af, 0x422d7a25, 0xbd8817db, 0x916e828c -> "motionstate_idle", "motionstate_walk", "motionstate_run", "motionstate_actionmode_idle", and "motionstate_actionmode_walk"
		//not really necessary
		AI::TASK_FORCE_MOTION_STATE(0, 0xbac0f10b, 0);
		for (int i =0; i < actorRecording.size(); i++) {
		ActorRecordingItem recordingItem = actorRecording[i];
		AI::TASK_GO_STRAIGHT_TO_COORD(0, recordingItem.getLocation().x, recordingItem.getLocation().y, recordingItem.getLocation().z, 2.0f, -1, recordingItem.getHeading(), 0);
		}
		AI::CLOSE_SEQUENCE_TASK(actorSeq);
		AI::TASK_PERFORM_SEQUENCE(actorPed, actorSeq);
		AI::CLEAR_SEQUENCE_TASK(&actorSeq);
		*/

		//TaskSequence actorSeq;
		//AI::OPEN_SEQUENCE_TASK(&actorSeq);

		//AI::CLEAR_PED_TASKS_IMMEDIATELY(actorPed);
		//AI::CLEAR_PED_TASKS(actorPed);
		
		/*
		AI::TASK_FLUSH_ROUTE();

		for (int i = 0; i < actorRecording.size(); i++) {
			ActorRecordingItem recordingItem = actorRecording[i];
			AI::TASK_EXTEND_ROUTE(recordingItem.getLocation().x, recordingItem.getLocation().y, recordingItem.getLocation().z);
		
			log_to_file("AI::TASK_EXTEND_ROUTE(" + std::to_string(recordingItem.getLocation().x) + "," + std::to_string(recordingItem.getLocation().y) + "," + std::to_string(recordingItem.getLocation().z) + ");");
		}
		AI::TASK_FOLLOW_POINT_ROUTE(actorPed, 2.0f, 0);
		*/


		

		/*
		TaskSequence actorSeq;
		AI::OPEN_SEQUENCE_TASK(&actorSeq);
		
		AI::TASK_FOLLOW_POINT_ROUTE(0, 2.0f, 0);
		AI::CLOSE_SEQUENCE_TASK(actorSeq);
		AI::TASK_PERFORM_SEQUENCE(actorPed, actorSeq);
		AI::CLEAR_SEQUENCE_TASK(&actorSeq);*/
		

		for (int i = 0; i < actorRecording.size(); i++) {
			ActorRecordingItem recordingItem = actorRecording[i];

			Vector3 targetLocation = recordingItem.getLocation();
			playback_recording_to_waypoint(actorPed, targetLocation);
			bool isInVehicle = PED::IS_PED_IN_ANY_VEHICLE(actorPed, 0);
			bool isPedInHeli = PED::IS_PED_IN_ANY_HELI(actorPed);
			bool isPedInPlane= PED::IS_PED_IN_ANY_PLANE(actorPed);
			bool isPedInBoat = PED::IS_PED_IN_ANY_BOAT(actorPed);

			float minDistance = 4.0;

			if (isPedInHeli) {
				minDistance = 50.0;
			}else if (isPedInPlane) {
				minDistance = 100.0;
			}
			else if (isPedInBoat) {
				minDistance = 60.0;
			}
			else if (isInVehicle) {
				minDistance = 15.0;
			}

			int counter = 0;
			while (counter<100) {
				Vector3 currentLocation = ENTITY::GET_ENTITY_COORDS(actorPed, 1);
				float distanceToTarget = SYSTEM::VDIST(currentLocation.x, currentLocation.y, currentLocation.z, targetLocation.x, targetLocation.y, targetLocation.z);

				log_to_file("Distance " + std::to_string(distanceToTarget));

				if (distanceToTarget < minDistance) {
					log_to_file("Next waypoint");
					break;
				}

				WAIT(300);
				counter++;
			}

		}




		set_status_text("Recording stopped");
		nextWaitTicks = 400;

	}


}

void action_submenu_active_selected() {
	//switch to actor
	if (submenu_active_action == SUBMENU_ITEM_RECORD_PLAYER) {
		action_record_scene_for_actor();
	}
	else if (submenu_active_action == SUBMENU_ITEM_REMOVE_FROM_SLOT) {
		//menu_active_action is the index of the actor from the main menu
		action_remove_actor_from_index(menu_active_action);
		submenu_is_displayed = false;
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
	else if (submenu_active_action == SUBMENU_ITEM_SPOT_LIGHT) {
		action_next_spot_light();
	}
	else if (submenu_active_action == SUBMENU_ITEM_SPOT_LIGHT_COLOR) {
		action_next_spot_light_color();
	}
	else if (submenu_active_action == SUBMENU_ITEM_DRUNK) {
		action_next_walking_style();
	}
		
}

void action_menu_active_selected() {
	//switch to actor
	if (menu_active_action >= 1 && menu_active_action <= 9) {
		swap_to_actor_with_index(menu_active_action);
		//force the active menu to be set to the new actor
		menu_active_index = -1;
		menu_active_ped = PLAYER::PLAYER_PED_ID();
	}
	else if (menu_active_action == MENU_ITEM_AUTOPILOT) {
		//autpilot is cancelled by switching to current actor
		int currentIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
		if (currentIndex != -1) {
			swap_to_actor_with_index(currentIndex);
		}
	}
	else if (menu_active_action == MENU_ITEM_CHASE) {
		action_vehicle_chase();
	}
	else if (menu_active_action == MENU_ITEM_ESCORT) {
		action_vehicle_escort();
	}
	else if (menu_active_action == MENU_ITEM_SCENE_MODE) {
		action_toggle_scene_mode();
	}
	else if (menu_active_action == MENU_ITEM_ADD_TO_SLOT) {
		add_ped_to_slot(get_next_free_slot(), PLAYER::PLAYER_PED_ID());
		//force the active menu to be set to the new actor
		menu_active_index = -1;
		menu_active_ped = PLAYER::PLAYER_PED_ID();
	}
	else if (menu_active_action == MENU_ITEM_ADD_CLONE_TO_SLOT && actor0IsClone) {
		if (get_index_for_actor(actorShortcut[0] == -1)) {
			int slot = get_next_free_slot();
			if (slot != -1) {
				add_ped_to_slot(slot, actorShortcut[0]);
				actor0IsClone = false;
				swap_to_actor_with_index(slot);
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


	char key_menu_down_str[256], key_menu_up_str[256], key_menu_select_str[256], key_menu_left_str[256], key_menu_right_str[256];
	GetPrivateProfileString("keys", "key_menu_down", "NUM2", key_menu_down_str, sizeof(key_menu_down_str), config_path);
	GetPrivateProfileString("keys", "key_menu_up", "NUM8", key_menu_up_str, sizeof(key_menu_up_str), config_path);
	GetPrivateProfileString("keys", "key_menu_select", "NUM5", key_menu_select_str, sizeof(key_menu_select_str), config_path);
	GetPrivateProfileString("keys", "key_menu_left", "NUM4", key_menu_left_str, sizeof(key_menu_left_str), config_path);
	GetPrivateProfileString("keys", "key_menu_right", "NUM6", key_menu_right_str, sizeof(key_menu_right_str), config_path);


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
	if (key_menu_left == 0) {
		log_to_file(std::string(key_menu_right_str) + " is not a valid key");
		key_menu_right = str2key("NUM6");
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

bool hud_toggle_key_pressed()
{
	return IsKeyJustUp(key_hud);
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



void menu_action_up() {
	if(submenu_is_active == false){
		menu_active_index++;
		if (menu_active_index > menu_max_index) {
			menu_active_index = menu_max_index;
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

void menu_action_down() {
	if (submenu_is_active == false) {
		menu_active_index--;
		if (menu_active_index < 0) {
			menu_active_index = 0;
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

void action_copy_player_actions() {


	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	if (actorIndex == -1) {
		set_status_text("Actor must be assigned slot 1-9 before recording actions");
	}
	else {
		is_firing_squad_engaged = true;
		set_status_text("Firing squad mode: Actors will now copy the actions of the player");
		Ped playerPed = actorShortcut[actorIndex];

		WAIT(500);

		bool bCopying = true;
		DWORD tickStart = GetTickCount();
		DWORD tickLast = tickStart;
		DWORD tickNow = tickStart;
		CONST DWORD DELTA_TICKS = 10;
		float previousHeading;
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
		for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
			if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(actorShortcut[i], true);
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

				/* Will not update heading
				float actorHeading = ENTITY::GET_ENTITY_HEADING(playerPed);
				if (actorHeading != previousHeading) {
				//log_to_file("Changing heading to " + std::to_string(actorHeading));
				previousHeading = actorHeading;
				for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
				if (actorShortcut[i] != 0 && actorShortcut[i]!= playerPed) {
				//log_to_file("Changing heading to " + std::to_string(actorHeading) + " for actor " + std::to_string(actorShortcut[i]));
				PED::SET_PED_DESIRED_HEADING(actorShortcut[i], actorHeading);
				}
				}
				}*/

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
						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
								if (currentScenario.hasEnterAnim) {
									AI::TASK_START_SCENARIO_IN_PLACE(actorShortcut[i], currentScenario.name, -1, 1);
								}
								else {
									AI::TASK_START_SCENARIO_IN_PLACE(actorShortcut[i], currentScenario.name, -1, 0);
								}
								
							}
						}
					}
				}
				else if (isPedUsingScenario) {
					log_to_file("Removing scenario from actors");
					isPedUsingScenario = false;

					for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
						if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
							AI::CLEAR_PED_TASKS(actorShortcut[i]);
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
						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
								if (isSkydiving == false) {
									log_to_file("Giving weapon " + std::to_string(currentWeapon) + " to actor " + std::to_string(actorShortcut[i]));
									WEAPON::GIVE_WEAPON_TO_PED(actorShortcut[i], currentWeapon, 1000, 1, 1);
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
					PLAYER::_GET_AIMED_ENTITY(PLAYER::PLAYER_ID(), &targetEntity);
					log_to_file("Player aiming at " + std::to_string(targetEntity));
					//if new target which exist, make all actors aim at it
					if (ENTITY::DOES_ENTITY_EXIST(targetEntity) && targetEntity != currentTarget) {
						currentTarget = targetEntity;
						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {

								if (isSkydiving == false) {//TASK_AIM_GUN_AT_ENTITY while skydiving has a tiny sideeffect
														   //log_to_file("Aim at " + std::to_string(targetEntity) + " for actor " + std::to_string(actorShortcut[i]));
									if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
										//doesn't work :(
										AI::TASK_VEHICLE_AIM_AT_PED(lastVehicle, targetEntity);

									}
									else {
										AI::TASK_AIM_GUN_AT_ENTITY(actorShortcut[i], targetEntity, -1, 0);
									}

								}
							}
						}
					}

					if (PED::IS_PED_SHOOTING(playerPed)) {
						isShooting = true;
						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed && actorShortcut[i] != currentTarget) {
								//log_to_file("Shoot at " + std::to_string(targetEntity) + " for actor " + std::to_string(actorShortcut[i]));
								if (isSkydiving == false) {//TASK_SHOOT_AT_ENTITY while skydiving has a tiny sideeffect
									if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0)) {
										//doesn't work :(
										AI::TASK_VEHICLE_SHOOT_AT_PED(lastVehicle, targetEntity, 0x41a00000);
									}
									else {
										AI::TASK_SHOOT_AT_ENTITY(actorShortcut[i], currentTarget, -1, GAMEPLAY::GET_HASH_KEY("FIRING_PATTERN_SINGLE_SHOT"));
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
					for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
						if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {

							if (isSkydiving == false) {//clear_ped_tasks while skydiving has a tiny sideeffect
								log_to_file("Clearing tasks for actor " + std::to_string(actorShortcut[i]));
								AI::CLEAR_PED_TASKS(actorShortcut[i]);
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

						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
								log_to_file("Actor " + std::to_string(actorShortcut[i]) + " should enter vehicle " + std::to_string(pedVehicle) + " in seat " + std::to_string(seatIndex));

								if (PED::IS_PED_IN_ANY_PLANE(playerPed)) {
									PED::SET_PED_INTO_VEHICLE(actorShortcut[i], pedVehicle, -2);
								}
								else {
									AI::TASK_ENTER_VEHICLE(actorShortcut[i], pedVehicle, -1, seatIndex, 1.0, 1, 0);
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
						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
								WEAPON::GIVE_WEAPON_TO_PED(actorShortcut[i], GAMEPLAY::GET_HASH_KEY("gadget_parachute"), 1, 1, 1);
								//AI::TASK_SKY_DIVE(actorShortcut[i]);
								AI::TASK_LEAVE_VEHICLE(actorShortcut[i], lastVehicle, 4160);
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
						GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(playerLocation.x, playerLocation.y, playerLocation.z, &zGroundLevel);

						//log_to_file("Ground level to parachute to is " +std::to_string(zGroundLevel));

						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed) {
								if (PED::GET_PED_PARACHUTE_STATE(actorShortcut[i]) == 0) {
									//log_to_file("Actor " + std::to_string(actorShortcut[i]) + " should deploy parachute");

									AI::TASK_PARACHUTE_TO_TARGET(actorShortcut[i], playerLocation.x, playerLocation.y, zGroundLevel);

								}
								else if (PED::IS_PED_IN_ANY_VEHICLE(actorShortcut[i], 0)) {
									//log_to_file("Actor " + std::to_string(actorShortcut[i]) + " should sky dive");
									//had some issues with them not exiting the vehicle so use TASK_LEAVE_VEHICLE with a teleport flag instead
									//AI::TASK_SKY_DIVE(actorShortcut[i]);
									AI::TASK_LEAVE_VEHICLE(actorShortcut[i], lastVehicle, 4160);
									WEAPON::GIVE_WEAPON_TO_PED(actorShortcut[i], GAMEPLAY::GET_HASH_KEY("gadget_parachute"), 1, 1, 1);
								}
							}
						}
						WAIT(50);
					}
				}
				else if (isSkydiving) {//check if we have landed
					if (PED::GET_PED_PARACHUTE_STATE(playerPed) == -1) {
						bool allActorsOnGround = true;
						for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
							if (actorShortcut[i] != 0 && actorShortcut[i] != playerPed && PED::GET_PED_PARACHUTE_STATE(actorShortcut[i]) != -1) {
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




void main()
{


	while (true)
	{
		/* ACTIONS WHICH MAY NEED TO WAIT A FEW TICKS */
		if (nextWaitTicks == 0 || GetTickCount() - mainTickLast >= nextWaitTicks) {
			//nextWaitTicks will be set by action methods in order to define how long before next input can be processed
			nextWaitTicks = 0;

			if (scene_teleport_to_start_locations_key_pressed()) {
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
					should_display_hud = true;
				}
			}


			if (autopilot_for_player_key_pressed()) {
				action_autopilot_for_player();
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

			/*
			if (increase_aggressiveness_key_pressed()) {
				action_increase_aggressiveness(PLAYER::PLAYER_PED_ID(), false);
			}

			if (decrease_aggressiveness_key_pressed()) {
				action_decrease_aggressiveness(PLAYER::PLAYER_PED_ID(),false);
			}

			if (increase_aggressiveness_for_all_actors_key_pressed()) {
				action_increase_aggressiveness_for_all_actors();
			}

			if (decrease_aggressiveness_for_all_actors_key_pressed()) {
				action_decrease_aggressiveness_for_all_actors();
			}*/


			if (enter_nearest_vehicle_as_passenger_key_pressed()) {
				action_enter_nearest_vehicle_as_passenger();
			}

			if (teleport_player_key_pressed()) {
				teleport_player_to_waypoint();
			}

			if (should_display_app_hud()) {
				if (menu_up_key_pressed()) {
					menu_action_up();
				} else if (menu_down_key_pressed()) {
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


			action_if_ped_assign_shortcut_key_pressed();

			action_if_ped_execute_shortcut_key_pressed();

			check_if_player_is_passenger_and_has_waypoint();

			mainTickLast = GetTickCount();
		}

		/* ACTIONS WHICH ARE PERFORMED EVERY TICK */

		//Display HUD for app
		if (should_display_app_hud()) {
			draw_instructional_buttons();
			draw_menu();
		}

		draw_spot_lights();



		if (is_timlapse_active && GetTickCount() - timelapseLastTick > timelapseDeltaTicks) {
			action_timelapse_tick();
		}

		//check if the player is dead/arrested, in order to swap back to original in order to avoid crash
		check_player_model();

		//Wait for next tick
		WAIT(0);
	}
}

void ScriptMain()
{
	GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT("CommonMenu", 0);

	scaleForm = GRAPHICS::_REQUEST_SCALEFORM_MOVIE2("instructional_buttons");
	log_to_file("Waiting for instructional_buttons to load");
	while (!GRAPHICS::HAS_SCALEFORM_MOVIE_LOADED(scaleForm)) {
		WAIT(0);
	}
	log_to_file("instructional_buttons have loaded");

	set_status_text("Scene director 1.2.2 by elsewhat");
	set_status_text("Scene is setup mode");
	init_read_keys_from_ini();

	gtaScenarios = getAllGTAScenarios();
	gtaWeatherTypes = getAllGTAWeather();
	gtaWalkingStyles = getAllMovementClipSet();
	for (auto & walkingStyle : gtaWalkingStyles) {
		STREAMING::REQUEST_CLIP_SET(walkingStyle.id);
	}
	
	

	create_relationship_groups();
	//log_to_file("Screen Director initialized");
	//log_to_file("Value of test property from config file: " + std::to_string(test_property));
	main();
}