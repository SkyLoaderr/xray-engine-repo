#include "stdafx.h"
#include "holder_custom.h"
#include "script_space.h"
using namespace luabind;

void CHolderCustom::script_register(lua_State *L)
{
	module(L)
		[
			class_<CHolderCustom>("holder")
			.def("engaged",			&CHolderCustom::Engaged)
		];
}