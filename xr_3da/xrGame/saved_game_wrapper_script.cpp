////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper_script.cpp
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "saved_game_wrapper.h"
#include "script_space.h"
#include "ai_space.h"
#include "game_graph.h"

using namespace luabind;

LPCSTR CSavedGameWrapper__level_name	(const CSavedGameWrapper *self)
{
	return			(*ai().game_graph().header().level(self->level_id()).name());
}

void CSavedGameWrapper::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CSavedGameWrapper>("CSavedGameWrapper")
			.def(constructor<LPCSTR>())
			.def("game_time",	&CSavedGameWrapper::game_time)
			.def("level_id",	&CSavedGameWrapper::level_id)
			.def("level_name",	&CSavedGameWrapper__level_name)
	];
}
