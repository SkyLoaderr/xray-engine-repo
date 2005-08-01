#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManagerScript.h"
//#include "../MainUI.h"

#include "../script_space.h"
#include <luabind\adopt_policy.hpp>

using namespace luabind;

void CUIOptionsManagerScript::SetCurrentValues(const char* group){
	CUIOptionsItem::GetOptionsManager()->SetCurrentValues(group);
}

void CUIOptionsManagerScript::SaveValues(const char* group){
	CUIOptionsItem::GetOptionsManager()->SaveValues(group);
}

void CUIOptionsManagerScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<CUIOptionsManagerScript>("COptionsManager")
			.def(							constructor<>())
			.def("SetCurrentValues",	CUIOptionsManagerScript::SetCurrentValues )
			.def("SaveValues",			CUIOptionsManagerScript::SaveValues )
		

		];
}