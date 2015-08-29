#include "script.h"
#include "keyboard.h"
#include "utils.h"

#include <string>
#include <ctime>
#include <vector>
#include <fstream>


//attributes to the actors in the slot 1-9 
//index 0 is reserved for the last actor
Ped actorShortcut[10] = {};
//waypoint for the acto
bool actorHasWaypoint[10] = {};
Vector3 actorWaypoint[10] = {};
//location when scene was set to active last time
bool actorHasStartLocation[10] = {};
Vector3 actorStartLocation[10] = {};
float actorStartLocationHeading[10] = {};
float actorDriverAgressiveness[] = { 0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f,0.8f };

int blipIdShortcuts[10] = {};

bool is_autopilot_engaged_for_player = false;
bool is_chase_player_engaged = false;
int chase_player_index = -1;
bool is_escort_player_engaged = false;
int escort_player_index = -1;

DWORD actorHashGroup = 0x5F0783F1;
Hash* actorHashGroupP = &actorHashGroup;


enum SCENE_MODE {
	SCENE_MODE_ACTIVE = 1,
	SCENE_MODE_SETUP = 0
};

SCENE_MODE sceneMode = SCENE_MODE_ACTIVE;

//Represents the scene status of each actor
//1=Act on instructions immediately
//0=No action (scene setup mode)
//x=Not used (yet)
SCENE_MODE actorStatus[10] = { SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE,SCENE_MODE_ACTIVE };


//used in passenger waypoint
int lastWaypointID = -1;

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

int nextWaitTicks = 0;
int forceSlotIndexOverWrite = -1;

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
		char* filename = "screen_director.log";
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
/*
LPCSTR config_path = ".\\screen_director.ini";
int test_property = GetPrivateProfileInt("keys", "test_key", 42, config_path);
*/

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
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(1, actorHashGroup, 0x6F0783F5);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(1, actorHashGroup, actorHashGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, 0x6F0783F5, actorHashGroup);

}

void assign_actor_to_relationship_group(Ped ped) {
	log_to_file("Adding actor to relationship group");
	 
	if(PED::GET_PED_RELATIONSHIP_GROUP_HASH(ped) == actorHashGroup) {
		log_to_file("Ped already belongs to actor relationship group");
	}else {
		PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, actorHashGroup);
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
					AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, waypointCoord.z, vehicleMaxSpeed, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
					log_to_file("move_to_waypoint: Flying in heli with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));
					if (suppress_msgs != true) {
						set_status_text("Flying to waypoint");
					}
				}
				else if (PED::IS_PED_IN_ANY_PLANE(ped)) {
					//z dimension is on ground level so add a bit to it
					AI::TASK_PLANE_MISSION(pedDriver, pedVehicle, 0, 0, waypointCoord.x, waypointCoord.y, waypointCoord.z + 200.0, 4, 30.0, 50.0, -1, vehicleMaxSpeed, 50);

					//AI::TASK_VEHICLE_DRIVE_TO_COORD(pedDriver, pedVehicle, waypointCoord.x, waypointCoord.y, 500.0, vehicleMaxSpeed, 1, ENTITY::GET_ENTITY_MODEL(pedVehicle), 1, 5.0, -1);
					log_to_file("move_to_waypoint: Flying in plane with vehicle:" + std::to_string(pedVehicle) + " with max speed:" + std::to_string(vehicleMaxSpeed));
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

	if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(playerPed);
		PED::SET_PED_INTO_VEHICLE(clonedPed, vehicle, -2);
	}
	assign_actor_to_relationship_group(clonedPed);

	set_status_text("Cloned myself. Possess clone with ALT+0");
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
		//try to prevent fleeing during teleport
		AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(clonedPed, true);
		PED::SET_PED_FLEE_ATTRIBUTES(clonedPed, 0, 0);

		PED::SET_PED_INTO_VEHICLE(clonedPed, clonedVehicle, -1);

		WAIT(200);

		//assign to a slot if one exists
		int actorSlotIndex = get_next_free_slot();
		if (actorSlotIndex != -1) {
			actorShortcut[actorSlotIndex] = clonedPed;
			assign_actor_to_relationship_group(clonedPed);
			ensure_max_driving_ability(clonedPed);

			int blipId = UI::ADD_BLIP_FOR_ENTITY(clonedPed);
			blipIdShortcuts[actorSlotIndex] = blipId;

			//BLIP Sprite for nr1=17, nr9=25
			UI::SET_BLIP_SPRITE(blipId, 16 + actorSlotIndex);

			set_status_text("Cloned player and vehicle into slot ALT+" + std::to_string(actorSlotIndex));
		}
		else {
			set_status_text("Cloned player and vehicle (but found no free actor slot)");
		}

		WAIT(500);
		AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(clonedPed, false);

		nextWaitTicks = 200;
	}
	else {
		set_status_text("ALT+F10 clone requires that you're in a vehicle");
	}

}


void enter_nearest_vehicle_as_passenger() {
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

			//check if there exist an actor for this index
			if (actorShortcut[pedShortcutsIndex] != 0) {
				if (forceSlotIndexOverWrite != pedShortcutsIndex) {
					log_to_file("action_if_ped_assign_shortcut_key_pressed: Slot already exists");
					set_status_text("Actor already exist in slot. CTRL+" + std::to_string(pedShortcutsIndex) + " once more to overwrite");
					forceSlotIndexOverWrite = pedShortcutsIndex;
					return;
				}
				else {
					log_to_file("action_if_ped_assign_shortcut_key_pressed: Slot will be overwritten");
					forceSlotIndexOverWrite = -1;
					//Remove old blip
					int blipIdsToRemove[1] = { blipIdShortcuts[pedShortcutsIndex] };
					UI::REMOVE_BLIP(blipIdsToRemove);
					//and continue storing the actor
				}

			}

			Ped playerPed = PLAYER::PLAYER_PED_ID();
			actorShortcut[pedShortcutsIndex] = playerPed;

			assign_actor_to_relationship_group(playerPed);

			int blipId = UI::ADD_BLIP_FOR_ENTITY(playerPed);
			blipIdShortcuts[pedShortcutsIndex] = blipId;
			//BLIP Sprite for nr1=17, nr9=25
			UI::SET_BLIP_SPRITE(blipId, 16 + pedShortcutsIndex);

			//Store current waypoint
			if (UI::IS_WAYPOINT_ACTIVE()) {
				int waypointID = UI::GET_FIRST_BLIP_INFO_ID(UI::_GET_BLIP_INFO_ID_ITERATOR());
				Vector3 waypointCoord = UI::GET_BLIP_COORDS(waypointID);
				actorWaypoint[pedShortcutsIndex] = waypointCoord;
				actorHasWaypoint[pedShortcutsIndex] = true;
			}

			ensure_ped_and_vehicle_is_not_deleted(playerPed);

			ensure_max_driving_ability(playerPed);

			log_to_file("action_if_ped_assign_shortcut_key_pressed: Stored current ped in slot " + std::to_string(pedShortcutsIndex));
			set_status_text("Stored current ped. Retrieve with ALT+" + std::to_string(pedShortcutsIndex));

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
				log_to_file("Autopilot engaged for player. ALT+" + std::to_string(actorIndex));
				set_status_text("Autopilot engaged for player. ALT+" + std::to_string(actorIndex) + " to turn it off");

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

void action_toggle_scene_mode() {
	if (sceneMode == SCENE_MODE_ACTIVE) {
		sceneMode = SCENE_MODE_SETUP;
		set_status_text("Scene is now in setup mode. Press ALT+SPACE to active all actors. Press ALT+DEL to teleport actors back to start location");
		log_to_file("SCENE SETUP");
	}
	else {
		sceneMode = SCENE_MODE_ACTIVE;
		set_status_text("Scene is now active! Press ALT+SPACE for setup mode");
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


bool copy_player_actions_key_pressed() {
	//ALT+ C
	if (IsKeyDown(VK_MENU) && IsKeyDown(0x43)) {
		return true;
	}
	else {
		return false;
	}
}

void action_copy_player_actions() {


	int actorIndex = get_index_for_actor(PLAYER::PLAYER_PED_ID());
	if (actorIndex == -1) {
		set_status_text("Actor must be assigned slot 1-9 before recording actions");
	}
	else {
		set_status_text("Actors will now copy the actions of the player");
		Ped playerPed = actorShortcut[actorIndex];

		WAIT(500);

		bool bCopying = true;
		DWORD tickStart = GetTickCount();
		DWORD tickLast = tickStart;
		DWORD tickNow = tickStart;
		CONST DWORD DELTA_TICKS = 10;
		float previousHeading;

		//main loop
		while (bCopying == true) {
			tickNow = GetTickCount();

			//check only once pr DELTA_TICKS
			if (tickNow - tickLast >= DELTA_TICKS) {


				Vector3 actorLocation = ENTITY::GET_ENTITY_COORDS(playerPed, true);

				float actorHeading = ENTITY::GET_ENTITY_HEADING(playerPed);
				if (actorHeading != previousHeading) {
					log_to_file("Changing heading to " + std::to_string(actorHeading));
					previousHeading = actorHeading;
					for (int i = 1; i < sizeof(actorShortcut) / sizeof(Ped); i++) {
						if (actorShortcut[i] != 0 && actorShortcut[i]!= playerPed) {
							log_to_file("Changing heading to " + std::to_string(actorHeading) + " for actor " + std::to_string(actorShortcut[i]));
							PED::SET_PED_DESIRED_HEADING(actorShortcut[i], actorHeading);
						}
					}
				}


				/*
				if (PLAYER::IS_PLAYER_FREE_AIMING(playerPed)) {
					Entity targetEntity;
					PLAYER::_GET_AIMED_ENTITY(playerPed, &targetEntity);

					if (ENTITY::DOES_ENTITY_EXIST(targetEntity)) {

					}

				}*/

				tickLast = tickNow;
			}



			if (copy_player_actions_key_pressed()) {
				bCopying = false;
			}
			WAIT(0);
		}
		
		set_status_text("Copying of actions stopped");
		nextWaitTicks = 400;

	}


}


bool possess_key_pressed()
{
	return IsKeyJustUp(VK_F9);
}

bool clone_key_pressed()
{
	return IsKeyJustUp(VK_F10);
}

bool clone_player_with_vehicle_key_pressed()
{
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_F10)) {
		return true;
	}
	else {
		return false;
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

bool scene_mode_toggle_key_pressed() {
	//ALT+SPACE
	if (IsKeyDown(VK_MENU) && IsKeyDown(VK_SPACE)) {
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





void main()
{
	while (true)
	{
		if (scene_mode_toggle_key_pressed()) {
			action_toggle_scene_mode();
		}

		if (scene_teleport_to_start_locations_key_pressed()) {
			action_teleport_to_start_locations();
		}

		if (set_same_waypoint_for_all_actors_key_pressed()) {
			action_set_same_waypoint_for_all_actors();
		}

		if (possess_key_pressed()) {
			action_possess_ped();
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



		if (clone_player_with_vehicle_key_pressed()) {
			action_clone_player_with_vehicle();
		}
		else if (clone_key_pressed()) {
			action_clone_player();
		}


		if (enter_nearest_vehicle_as_passenger_key_pressed()) {
			enter_nearest_vehicle_as_passenger();
		}

		if (teleport_player_key_pressed()) {
			teleport_player_to_waypoint();
		}

		action_if_ped_assign_shortcut_key_pressed();

		action_if_ped_execute_shortcut_key_pressed();

		check_if_player_is_passenger_and_has_waypoint();

		//check if the player is dead/arrested, in order to swap back to original
		check_player_model();

		WAIT(nextWaitTicks);
		nextWaitTicks = 0;
	}
}

void ScriptMain()
{
	set_status_text("Scene director 1.0.2 by elsewhat");
	set_status_text("Scene is now active! Press ALT+SPACE for setup mode");
	create_relationship_groups();
	//log_to_file("Screen Director initialized");
	//log_to_file("Value of test property from config file: " + std::to_string(test_property));
	main();
}