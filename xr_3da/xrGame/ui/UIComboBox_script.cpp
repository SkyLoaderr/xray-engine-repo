// File:        UIComboBox_script.cpp
// Description: exports CUIComobBox to LUA environment
// Created:     11.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#include "stdafx.h"
#include "UIComboBox.h"

#include "../script_space.h"

using namespace luabind;

void CUIComboBox::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIComboBox, CUIWindow>("CUIComboBox")
		.def(						constructor<>())
		.def("Init",				(void (CUIComboBox::*)(int, int, int))   CUIComboBox::Init)		
		.def("SetVertScroll",		&CUIComboBox::SetVertScroll)
		.def("SetListLength",		&CUIComboBox::SetListLength)
		.def("AddItem",				(void (CUIComboBox::*)(LPCSTR, bool)) CUIComboBox::AddItem)
		.def("AddItem",				(void (CUIComboBox::*)(LPCSTR)) CUIComboBox::AddItem)
	];
}