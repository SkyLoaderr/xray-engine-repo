////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_space.h"
#include "script_game_object.h"

using namespace luabind;

extern class_<CScriptGameObject> &script_register_game_object1(class_<CScriptGameObject> &);
extern class_<CScriptGameObject> &script_register_game_object2(class_<CScriptGameObject> &);

void CScriptGameObject::script_register(lua_State *L)
{
	class_<CScriptGameObject>	instance("game_object");

	module(L)
	[
		script_register_game_object2(
			script_register_game_object1(instance)
		)
	];
}