#include "stdafx.h"
#include "UITabControl.h"

#include "../script_space.h"

using namespace luabind;

void CUITabControl::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUITabControl, CUIWindow>("UITabControl")
		.def(					constructor<>())
		.def("AddItem",			(bool (CUITabControl::*)(CUIButton*))	&CUITabControl::AddItem)
		.def("AddItem",			(bool (CUITabControl::*)(const char*, const char*,int,int, int,int))	&CUITabControl::AddItem)
		.def("RemoveItem",		&CUITabControl::RemoveItem)
		.def("RemoveAll",		&CUITabControl::RemoveAll)
		.def("GetActiveIndex",	&CUITabControl::GetActiveIndex)
		.def("GetTabsCount",	&CUITabControl::GetTabsCount)
		.def("SetNewActiveTab",	&CUITabControl::SetNewActiveTab)
		.def("SetActiveTextColor",				&CUITabControl::SetActiveTextColor)
		.def("GetActiveTextColor",				&CUITabControl::GetActiveTextColor)
		.def("SetGlobalTextColor",				&CUITabControl::SetGlobalTextColor)
		.def("GetGloablTextColor",				&CUITabControl::GetGloablTextColor)
		.def("SetActiveButtonColor",				&CUITabControl::SetActiveButtonColor)
		.def("GetActiveButtonColor",				&CUITabControl::GetActiveButtonColor)
		.def("SetGlobalButtonColor",				&CUITabControl::SetGlobalButtonColor)
		.def("GetGlobalButtonColor",				&CUITabControl::GetGlobalButtonColor)
//		.def("",				&CUITabControl::)
	];

}