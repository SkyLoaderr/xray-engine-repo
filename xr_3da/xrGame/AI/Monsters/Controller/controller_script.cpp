#include "stdafx.h"
#include "controller.h"
#include "../../../script_space.h"

using namespace luabind;

void CController::script_register(lua_State *L)
{
	module(L)
	[
		class_<CController,CGameObject>("CController")
			.def(constructor<>())
	];
}
