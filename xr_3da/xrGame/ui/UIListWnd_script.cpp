#include "stdafx.h"
#include "UIListWnd.h"

#include "../script_space.h"

using namespace luabind;

bool CUIListWnd::AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont)
{
	CUIString			s;
	s.SetText(str);

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
		.def("SetItemHeight",				&CUIListWnd::SetItemHeight)
/*		.def("",				&CUIListWnd::)
		.def("",				&CUIListWnd::)
		.def("",				&CUIListWnd::)
		.def("",				&CUIListWnd::)
*/
		];
}