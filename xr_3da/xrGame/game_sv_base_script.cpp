////////////////////////////////////////////////////////////////////////////
//	Module 		: game_sv_base_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Base server game script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_sv_base.h"
#include "script_space.h"
#include "level.h"

using namespace luabind;

double get_time()
{
	return((double)Level().GetGameTime());
}

void game_sv_GameState::script_register(lua_State *L)
{
	module(L,"game")
	[
		// declarations
		def("time",								get_time),
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
	
	luabind::class_< game_sv_GameState, game_GameState >("game_sv_GameState")
	.def("get_id",				&game_sv_GameState::get_id)
	.def("get_it",				&game_sv_GameState::get_it)
	.def("get_it_2_id",			&game_sv_GameState::get_it_2_id)
	.def("get_name_it",			&game_sv_GameState::get_name_it)
	.def("get_name_id",			&game_sv_GameState::get_name_id)
	.def("get_count",			&game_sv_GameState::get_count)
	.def("get_id_2_eid",		&game_sv_GameState::get_id_2_eid)

	.def("get_option_i",		&game_sv_GameState::get_option_i)
	.def("get_option_s",		&game_sv_GameState::get_option_s)
	];
}
