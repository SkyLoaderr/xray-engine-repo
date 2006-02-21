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
#include "date_time.h"

using namespace luabind;

xr_string CSavedGameWrapper__game_time		(const CSavedGameWrapper *self)
{
	u32				years, months, days, hours, minutes, seconds, milliseconds;
	split_time		(self->game_time(), years, months, days, hours, minutes, seconds, milliseconds);
	string256		temp;
	sprintf			(temp,"%d:%d:%d:%d:%d:%d",years, months, days, hours, minutes, seconds, milliseconds);
	return			(temp);
}

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
			.def("game_time",	&CSavedGameWrapper__game_time)
			.def("level_name",	&CSavedGameWrapper__level_name)
			.def("level_id",	&CSavedGameWrapper::level_id)
	];
}
