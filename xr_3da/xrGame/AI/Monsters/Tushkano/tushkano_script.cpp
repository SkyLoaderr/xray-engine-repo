#include "stdafx.h"
#include "tushkano.h"
#include "../../../script_space.h"

using namespace luabind;

void CTushkano::script_register(lua_State *L)
{
	module(L)
		[
			class_<CTushkano,CGameObject>("CTushkano")
			.def(constructor<>())
		];
}
