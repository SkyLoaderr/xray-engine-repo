////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine_script.cpp
//	Created 	: 03.12.2004
//  Modified 	: 03.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Scripting engine script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_engine.h"
#include "script_space.h"

using namespace luabind;

void quit()
{
	exit (0);
}

void CScriptEngine::script_register(lua_State *L)
{
	module(L)
	[
		def("quit",&quit),
		def("exit",&quit)
	];
}
