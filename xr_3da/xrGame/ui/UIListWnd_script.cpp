#include "stdafx.h"
#include "UIListWnd.h"
#include "UIListItemEx.h"
#include "ServerList.h"

#include "../script_space.h"
#include <luabind\adopt_policy.hpp>

using namespace luabind;

bool CUIListWnd::AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont,bool doParse)
{
	CUIString			s;
	float shift_ = float(shift);
	s.SetText(str);
	if(doParse){
		CUIStatic::PreprocessText(s.m_str,GetWidth()-shift-5,pFont);
	}

	return AddParsedItem<CUIListItem>(s, shift_, color, pFont);

}

bool CUIListWnd::AddItem_script(CUIListItem* item){
	return AddItem(item, -1);
}

void CUIListWnd::script_register(lua_State *L)
{

	module(L)
	[

		class_<CUIListWnd, CUIWindow>("CUIListWnd")
		.def(							constructor<>())
		.def("AddText",					&CUIListWnd::AddText_script)
		.def("AddItem",                 &CUIListWnd::AddItem_script, adopt(_2))
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
		.def(							constructor<>()),

		class_<SServerFilters>("SServerFilters")
		.def(							constructor<>())
		.def_readwrite("empty",				&SServerFilters::empty)
		.def_readwrite("full",				&SServerFilters::full)
		.def_readwrite("with_pass",			&SServerFilters::with_pass)
		.def_readwrite("without_pass",		&SServerFilters::without_pass)
		.def_readwrite("without_ff",		&SServerFilters::without_ff)
		.def_readwrite("without_pb",		&SServerFilters::without_pb)
		.def_readwrite("listen_servers",	&SServerFilters::listen_servers),

		class_<CServerList, CUIWindow>("CServerList")
		.def(							constructor<>())
		.def("RefreshList",				&CServerList::RefreshGameSpyList)
		.def("ConnectToSelected",		&CServerList::ConnectToSelected)
		.def("SetFilters",				&CServerList::SetFilters)
		
		

		];
}