// File:        UIListWndEx_script.cpp
// Description: Exports to script
//              ListWndEx, ListItemEx, ListItemPair
// Created:     10.11.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua

// Copyright:   2004 GSC Game World

#include "stdafx.h"
#include "UIListWndEx.h"
#include "UIListItemPair.h"
#include "../script_space.h"

using namespace luabind;

bool CUIListWndEx::AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont,bool doParse)
{
	CUIString			s;
	s.SetText(str);
	if(doParse){
		CUIStatic::PreprocessText(s.m_str,GetWidth()-shift-5,pFont);
	}

	return AddParsedItem<CUIListItemEx>(s, shift, color, pFont);

}

void CUIListWndEx::script_register(lua_State *L)
{

	module(L)
		[
			class_<CUIListWndEx, CUIListWnd>("CUIListWndEx")
			.def("AddText",					&CUIListWndEx::AddText_script)
			.def(							constructor<>()),
			
			class_<CUIListItemEx, CUIListItem>("CUIListItemEx")
			.def(							constructor<>())
//			.def("SetPerformTextLimit",		&CUIListItemPair::SetPerformTextLimit),


			class_<CUIListItemPair, CUIListItemEx>("CUIListItemPair")
			.def(							constructor<>())
			.def("SetTextFileName",			&CUIListItemPair::SetTextFileName)
			.def("SetTextDateTime",			&CUIListItemPair::SetTextDateTime)
			.def("SetFontFileName",			&CUIListItemPair::SetFontFileName)
			.def("SetFontDateTime",			&CUIListItemPair::SetFontDateTime)
			.def("SetTextFileName",			&CUIListItemPair::SetTextFileName)
			.def("GetBorder",				&CUIListItemPair::GetBorder)
			.def("SetBorder",				&CUIListItemPair::SetBorder)

		];
}