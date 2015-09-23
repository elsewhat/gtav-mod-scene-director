#include "clipset_movement.h"
#include <vector>

std::vector<ClipSet> gtaClipsetMovements = {
	{"MOVE_M@DRUNK@SLIGHTLYDRUNK", "Tipsy 1" },
	{ "MOVE_M@BAIL_BOND_NOT_TAZERED","Tipsy 2" },
	{"MOVE_M@DRUNK@MODERATEDRUNK", "Drunk 1" },
	{"MOVE_M@DRUNK@MODERATEDRUNK_HEAD_UP","Drunk 2"  },
	{ "MOVE_M@DRUNK@VERYDRUNK","Very drunk" },
	{ "MOVE_M@BAIL_BOND_TAZERED","Tazered" },
	{ "move_heist_lester","Cane" },
	{ "move_injured_generic","Limp" },
	{ "move_ped_crouched","Crouched" },
	{ "ANIM_GROUP_MOVE_BALLISTIC","Heavy" },
	{ "MOVE_F@FEMME@","Female" },
	{ "MOVE_F@POSH@","Female posh" },
	{ "move_f@sexy@a","Female sexy" },
	{ "MOVE_F@TOUGH_GUY@","Female tough" },
	{ "MOVE_F@GANGSTER@NG","Female gangster" },
	{ "move_f@scared","Female scared" },
	{ "move_f@flee@a","Female flee" },
	{ "FEMALE_FAST_RUNNER","Female runner" },
	{ "move_m@brave","Male brave" },
	{ "move_m@casual@d","Male casual" },
	{ "MOVE_M@POSH@","Male posh" },
	{ "MOVE_M@TOUGH_GUY@","Male tough" },
	{ "move_m@gangster@var_f","Slouched" },
	{ "move_m@gangster@var_e","Gangster" },
	{ "MOVE_M@PRISON_GAURD","Prison guard" },
	{ "move_p_m_zero_janitor","Jantior" },
	{ "move_p_m_zero_slow","Male slow" },
	{ "clipset@move@trash_fast_turn","Garbageman" },



/* Not used movement clip sets
{ "move_characters@michael@fire","move_characters@michael@fire" },
{ "move_m@JOG@","move_m@JOG@" },
{ "move_characters@franklin@fire","move_characters@franklin@fire" },
{ "move_ped_mop","move_ped_mop" },
{ "missfbi4prepp1_garbageman","Garbageman 2" },
{ "move_lester_CaneUp","Cane 2" },
{ "ANIM_GROUP_MOVE_LEMAR_ALLEY","Lemar" },
{ "move_characters@Jimmy@slow@","Jimmy" },
{ "move_ped_bucket","move_ped_bucket" },
{ "MOVE_P_M_ONE","MOVE_P_M_ONE" },
{ "MOVE_P_M_ONE_BRIEFCASE","MOVE_P_M_ONE_BRIEFCASE" },
{ "move_m@gangster@var_i","Gangster_i" },
{ "move_m@fire","Male fire" },
{ "move_m@bag","move_m@bag" },
{ "MOVE_M@GANGSTER@NG","Male gangster" },
{ "MOVE_M@FEMME@","Male feminine" },
*/

};

std::vector<ClipSet> getAllMovementClipSet() {
	return gtaClipsetMovements;
}