#include "stdafx.h"
#include "Car.h"
#include "script_space.h"

using namespace luabind;

void CCar::script_register(lua_State *L)
{
	module(L)
	[
		class_<CCar,CGameObject>("CCar")
			.def(constructor<>())
	];
}