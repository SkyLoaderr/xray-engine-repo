#include "stdafx.h"
#include "xrServer_Objects.h"
#include "script_space.h"
#include "xrServer_script_macroses.h"

using namespace luabind;

void CSE_AbstractVisual::script_register(lua_State *L)
{
	module(L)[
		luabind_class_abstract2(
			CSE_AbstractVisual,
			"CSE_AbstractVisual",
			CSE_Visual,
			CSE_Abstract
			)
	];
}
/*
void CSE_Trigger::script_register(lua_State *L)
{
		module(L)[
			luabind_class_abstract1(
				CSE_Trigger,
				"cse_trigger",
				CSE_Abstract
				)
		];
}
*/