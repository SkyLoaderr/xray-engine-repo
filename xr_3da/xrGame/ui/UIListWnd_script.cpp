#include "stdafx.h"
#include "UIListWnd.h"
#include "UIListItemEx.h"

#include "../script_space.h"

using namespace luabind;

bool CUIListWnd::AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont,bool doParse)
{
	CUIString			s;
	s.SetText(str);
	if(doParse){
		CUIStatic::PreprocessText(s.m_str,GetWidth()-shift-5,pFont);
	}

	return AddParsedItem<CUIListItem>(s, shift, color, pFont);

}

void CUIListWnd::script_register(lua_State *L)
{

	module(L)
	[

		class_<CUIListWnd, CUIWindow>("CUIListWnd")
		.def(							constructor<>())
		.def("AddText",					&CUIListWnd::AddText_script)

		.def("RemoveItem",				&CUIListWnd::RemoveItem)
		.def("RemoveAll",				&CUIListWnd::RemoveAll)
		.def("EnableScrollBar",			&CUIListWnd::EnableScrollBar)
		.def("IsScrollBarEnabled",		&CUIListWnd::IsScrollBarEnabled)
		.def("ScrollToBegin",			&CUIListWnd::ScrollToBegin)
		.def("ScrollToEnd",				&CUIListWnd::ScrollToEnd)
		.def("SetItemHeight",			&CUIListWnd::SetItemHeight)
		.def("GetItem",					&CUIListWnd::GetItem)
		.def("GetItemPos",				&CUIListWnd::GetItemPos)
		.def("GetSize",				&CUIListWnd::GetSize)
		.def("ScrollToBegin",				&CUIListWnd::ScrollToBegin)
		.def("ScrollToEnd",				&CUIListWnd::ScrollToEnd)
		.def("ScrollToPos",				&CUIListWnd::ScrollToPos)
		.def("GetLongestSignWidth",				&CUIListWnd::GetLongestSignWidth)
		.def("SetWidth",				&CUIListWnd::SetWidth)
		.def("SetTextColor",				&CUIListWnd::SetTextColor)
		.def("ActivateList",				&CUIListWnd::ActivateList)
		.def("IsListActive",				&CUIListWnd::IsListActive)
		.def("SetVertFlip",				&CUIListWnd::SetVertFlip)
		.def("GetVertFlip",				&CUIListWnd::GetVertFlip)
		.def("SetFocusedItem",				&CUIListWnd::SetFocusedItem)
		.def("GetFocusedItem",				&CUIListWnd::GetFocusedItem)

		.def("GetSelectedItem",				&CUIListWnd::GetSelectedItem)
		.def("ResetFocusCapture",				&CUIListWnd::ResetFocusCapture)
		.def("SetRightIndention",				&CUIListWnd::SetRightIndention)
		.def("GetRightIndention",				&CUIListWnd::GetRightIndention),
/*		.def("",				&CUIListWnd::)
		.def("",				&CUIListWnd::)
		.def("",				&CUIListWnd::)*/

		class_<CUIListItem, CUIButton>("CUIListItem")
		.def(							constructor<>())

		];
}