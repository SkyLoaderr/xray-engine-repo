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
		def("time",								get_time)
//		def("get_surge_time",					Game::get_surge_time),
//		def("get_object_by_name",				Game::get_object_by_name),
	];
}
