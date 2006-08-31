#include "stdafx.h"
#include "poltergeist.h"
#include "../../../script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CPoltergeist::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPoltergeist,CGameObject>("CPoltergeist")
			.def(constructor<>())
	];
}
