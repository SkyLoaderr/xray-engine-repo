#include "stdafx.h"
#include "chimera.h"
#include "../../../script_space.h"

using namespace luabind;

void CChimera::script_register(lua_State *L)
{
	module(L)
	[
		class_<CChimera,CGameObject>("CChimera")
			.def(constructor<>())
	];
}
