#include "stdafx.h"
#include "bloodsucker.h"
#include "../../../script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CAI_Bloodsucker::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAI_Bloodsucker,CGameObject>("CAI_Bloodsucker")
			.def(constructor<>())
	];
}
