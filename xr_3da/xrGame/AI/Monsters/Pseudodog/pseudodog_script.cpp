#include "stdafx.h"
#include "pseudodog.h"
#include "../../../script_space.h"

using namespace luabind;

void CAI_PseudoDog::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAI_PseudoDog,CGameObject>("CAI_PseudoDog")
			.def(constructor<>())
	];
}
