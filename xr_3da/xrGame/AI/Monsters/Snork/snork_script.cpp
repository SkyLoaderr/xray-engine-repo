#include "stdafx.h"
#include "snork.h"
#include "../../../script_space.h"

using namespace luabind;

void CSnork::script_register(lua_State *L)
{
	module(L)
	[
		class_<CSnork,CGameObject>("CSnork")
			.def(constructor<>())
	];
}
