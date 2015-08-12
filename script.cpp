#include "script.h"
#include "keyboard.h"
#include "utils.h"

#include <string>
#include <ctime>
#include <vector>
#include <fstream>


Ped pedShortcuts[10] = {};


std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

void update_status_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.55, 0.55);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
		if (statusTextGxtEntry)
		{
			UI::_SET_TEXT_ENTRY((char *)statusText.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)statusText.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.5);
	}
}

void set_status_text(std::string str, DWORD time = 2500, bool isGxtEntry = false)
{
	statusText = str;
	statusTextDrawTicksMax = GetTickCount() + time;
	statusTextGxtEntry = isGxtEntry;
}

//log and config file handling borrowed from https://github.com/amoshydra/bearded-batman/blob/e814cf559edbb24b1ef80a326d0608ff67ba17cb/source/Kinky/script.cpp

void log_to_file(std::string message, bool bAppend = true) {
	if (1) {
		std::ofstream logfile;
		char* filename = "screen_director.log";
		if (bAppend)
			logfile.open(filename, std::ios_base::app);
		else
			logfile.open(filename);
		logfile << message + "\n";
		logfile.close();
	}
	return;
}

// Config file - test
LPCSTR config_path = ".\\screen_director.ini";
int test_property = GetPrivateProfileInt("keys", "test_key", 42, config_path);

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
			set_status_text("turning to normal");
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

void move_to_waypoint(Ped ped) {
	//code inspired by LUA plugin https://www.gta5-mods.com/scripts/realistic-vehicle-controls

	int waypointID;
	Vector3 waypointCoord;
	if (UI::IS_WAYPOINT_ACTIVE()) {
		waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
		waypointCoord = UI::GET_BLIP_COORDS(waypointID);
	}
	else {
		set_status_text("No waypoint active.");
		return;
	}

	if (PED::IS_PED_IN_ANY_VEHICLE(ped, 0)) {

		

		Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_USING(ped);

		//check if player is the driver
		Ped pedDriver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(playerVehicle, -1);
		if (pedDriver != ped) {
			set_status_text("Ped is not driver. Ignore waypoint");
		}
		else {
			AI::TASK_VEHICLE_DRIVE_TO_COORD(ped, playerVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, 100, 1, ENTITY::GET_ENTITY_MODEL(playerVehicle), 1, 5.0, -1);
			set_status_text("Driving to waypoint");
		}

	}
	else if (PED::IS_PED_ON_FOOT(ped)) {
		AI::TASK_GO_STRAIGHT_TO_COORD(ped, waypointCoord.x, waypointCoord.y, waypointCoord.z, 1.0f, -1, 27.0f, 0.5f);
		set_status_text("Walking to waypoint");
	}

}

void possess_ped(Ped swapToPed) {
	if (ENTITY::DOES_ENTITY_EXIST(swapToPed) && ENTITY::IS_ENTITY_A_PED(swapToPed)) {
		Ped swapFromPed = PLAYER::PLAYER_PED_ID();

		if (UI::IS_WAYPOINT_ACTIVE()) {
			move_to_waypoint(swapFromPed);
		}

		PLAYER::CHANGE_PLAYER_PED(PLAYER::PLAYER_ID(), swapToPed, false, false);
		
		//pause any AI tasks such as driving to waypoint
		AI::TASK_PAUSE(swapToPed, 1);
		
		pedShortcuts[0] = swapFromPed;

		//set_status_text("Possessing ped. Swap back with ALT+0");
	}
	else {
		set_status_text("Could not possess ped");
	}

}




void action_possess_ped() {
	Player player = PLAYER::PLAYER_ID();

	//If player is aiming at a ped, control that
	if (PLAYER::IS_PLAYER_FREE_AIMING(player)) {
		Entity targetEntity;
		PLAYER::_GET_AIMED_ENTITY(player, &targetEntity);

		if (ENTITY::DOES_ENTITY_EXIST(targetEntity) && ENTITY::IS_ENTITY_A_PED(targetEntity)) {
			set_status_text("Possessing targeted pedestrian");

			possess_ped(targetEntity);
			//give a pistol, so they can easily target the next person
			give_basic_weapon(targetEntity);
			
		}
		else {
			set_status_text("Aim on a pedestrian and try again");
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

			if (!ENTITY::DOES_ENTITY_EXIST(peds[offsettedID]))
			{
				continue;
			}

			Ped closestPed = peds[offsettedID];
			set_status_text("Possessing closest pedestrian");

			possess_ped(closestPed);

		}
		delete peds;
	}
}




void action_clone_myself() {
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	Ped clonedPed = PED::CLONE_PED(playerPed,0.0f, false, true);
	pedShortcuts[0] = clonedPed;

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		PED::SET_PED_INTO_VEHICLE(clonedPed, vehicle, -2);
	}

	set_status_text("Cloned myself. Possess clone with ALT+0");
}

void enter_nearest_vehicle_as_passenger() {
	set_status_text("Entering as passenger");
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 coord = ENTITY::GET_ENTITY_COORDS(playerPed, true);

	Vehicle vehicle = VEHICLE::GET_CLOSEST_VEHICLE(coord.x, coord.y, coord.z, 100.0, 0, 71);
	
	AI::TASK_ENTER_VEHICLE(playerPed, vehicle, -1, 0, 1.0, 1, 0);
}



bool possess_key_pressed()
{
	return IsKeyJustUp(VK_F9);
}

bool clone_key_pressed()
{
	return IsKeyJustUp(VK_F10);
}

bool swap_to_previous_possessed_key_pressed()
{
	//see https://msdn.microsoft.com/de-de/library/windows/desktop/dd375731(v=vs.85).aspx for key codes	
	return IsKeyDown(VK_CONTROL) && IsKeyDown(0x30);
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
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			pedShortcuts[pedShortcutsIndex] = playerPed;

			int blipId = UI::ADD_BLIP_FOR_ENTITY(playerPed);
			//BLIP Sprite for nr1=17, nr9=25
			UI::SET_BLIP_SPRITE(blipId, 16 + pedShortcutsIndex);

			set_status_text("Stored current ped. Retrieve with ALT+" + std::to_string(pedShortcutsIndex));
		}
	}
	WAIT(300);
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
		}else if (IsKeyDown(0x31)) {
			pedShortcutsIndex = 1;
		}else if (IsKeyDown(0x31)) {
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

			if (pedShortcuts[pedShortcutsIndex] == 0) {
				set_status_text("No stored ped. Store with CTRL+" + std::to_string(pedShortcutsIndex));
			}
			else {
				possess_ped(pedShortcuts[pedShortcutsIndex]);
			}
		}
	}
	WAIT(300);
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





void main()
{
	while (true)
	{
		update_status_text();

		if (possess_key_pressed()) {
			set_status_text("Possessing nearest pedestrian");
			action_possess_ped();
			WAIT(300);
		}

		if (clone_key_pressed()) {
			action_clone_myself();
			WAIT(300);
		}

		if (enter_nearest_vehicle_as_passenger_key_pressed()) {
			enter_nearest_vehicle_as_passenger();
			WAIT(300);
		}

		action_if_ped_assign_shortcut_key_pressed();

		action_if_ped_execute_shortcut_key_pressed();


		//check if the player is dead/arrested, in order to swap back to original
		check_player_model();

		WAIT(0);
	}
}

void ScriptMain()
{
	log_to_file("Screen Director initialized");
	log_to_file("Value of test property from config file: " + std::to_string(test_property));
	main();
}