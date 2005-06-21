#include "StdAfx.h"
#include "UIOptionsManagerScript.h"
#include "UIOptionsManager.h"
#include "../MainUI.h"

#include "../script_space.h"
#include <luabind\adopt_policy.hpp>

using namespace luabind;

void CUIOptionsManagerScript::SetDefaultValues(const char* group){
	UI()->OptionsManager()->SetDefaultValues(group);
}

void CUIOptionsManagerScript::SaveValues(const char* group){
	UI()->OptionsManager()->SaveValues(group);
}

void CUIOptionsManagerScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<CUIOptionsManagerScript>("COptionsManager")
			.def(							constructor<>())
			.def("SetDefaultValues",	CUIOptionsManagerScript::SetDefaultValues )
			.def("SaveValues",			CUIOptionsManagerScript::SaveValues )
		

		];
}