#include "StdAfx.h"
#include "UIOptionsItem.h"
#include "UIOptionsManagerScript.h"
//#include "../MainMenu.h"

#include "../script_space.h"
#include <luabind\adopt_policy.hpp>

using namespace luabind;

void CUIOptionsManagerScript::SetCurrentValues(const char* group){
	CUIOptionsItem::GetOptionsManager()->SetCurrentValues(group);
}

void CUIOptionsManagerScript::SaveValues(const char* group){
	CUIOptionsItem::GetOptionsManager()->SaveValues(group);
}

bool CUIOptionsManagerScript::IsGroupChanged(const char* group){
	return CUIOptionsItem::GetOptionsManager()->IsGroupChanged(group);
}

void CUIOptionsManagerScript::UndoGroup(const char* group){
	CUIOptionsItem::GetOptionsManager()->UndoGroup(group);
}

void CUIOptionsManagerScript::OptionsPostAccept(){
	CUIOptionsItem::GetOptionsManager()->OptionsPostAccept();
}

void CUIOptionsManagerScript::script_register(lua_State *L)
{
	module(L)
		[
			class_<CUIOptionsManagerScript>("COptionsManager")
			.def(							constructor<>())
			.def("SetCurrentValues",	CUIOptionsManagerScript::SetCurrentValues )
			.def("SaveValues",			CUIOptionsManagerScript::SaveValues )
			.def("IsGroupChanged",		CUIOptionsManagerScript::IsGroupChanged )
			.def("UndoGroup",			CUIOptionsManagerScript::UndoGroup )
			.def("OptionsPostAccept",	CUIOptionsManagerScript::OptionsPostAccept )
		];
}