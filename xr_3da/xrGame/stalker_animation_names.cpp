////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_names.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation names
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_names.h"

LPCSTR state_names			[] = {
	"cr_",
	"norm_",
	"dmg_norm_",
	0
};

LPCSTR weapon_names			[] = {
	"0_",
	"1_",
	"2_",
	"3_",
	"4_",
	"5_",
	"6_",
	"7_",
	"8_",
	"9_",
	"10_",
	0
};

LPCSTR weapon_action_names	[] = {
	"draw_",			// 0
	"attack_",			// 1
	"drop_",			// 2
	"holster_",			// 3
	"reload_",			// 4
	"pick_",			// 5
	"aim_",				// 6
	"walk_",			// 7
	"run_",				// 8
	"idle_",			// 9
	"prepare_",			// 10
	"strap_",			// 11
	"unstrap_",			// 12
	"look_beack_ls_",	// 13
	"look_beack_rs_",	// 14
	0
};

LPCSTR movement_names		[] = {
	"walk_",
	"run_",
	0
};

LPCSTR movement_action_names[] = {
	"fwd_",
	"back_",
	"ls_",
	"rs_",
	0
};

LPCSTR in_place_names		[] = {
	"idle_0",
	"idle_1",
	"turn_left_0",
	"turn_right_0",
	"turn_left_1",
	"turn_right_1",
	"jump_begin",
	"jump_idle",
	"jump_end",
	"jump_end_1",
	0
};

LPCSTR global_names			[] = {
	"damage_",
	"escape_",
	"dead_stop_",
	"hello_",
	0
};

LPCSTR head_names			[] = {
	"head_idle_0",
	"head_talk_0",
	0
};
