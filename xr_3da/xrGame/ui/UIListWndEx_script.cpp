#include "stdafx.h"
#include "UIListWndEx.h"
#include "UIListItemEx.h"
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

		];
}