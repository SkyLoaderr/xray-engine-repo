#include "stdafx.h"

#include "script_ui_registrator.h"
#include "UI\UIInventoryWnd.h"
#include "UI\UIStatsWnd.h"
#include "UI\UIMultiTextStatic.h"
#include "UIDMFragList.h"
#include "UIDMPlayerList.h"
#include "script_space.h"
#include <luabind\adopt_policy.hpp>
using namespace luabind;

void UIRegistrator::script_register(lua_State *L)
{
	module(L)
	[
		class_<RECT>("RECT")
		.def(					constructor<>())
		.def_readwrite("right",					&RECT::right)
		.def_readwrite("left",					&RECT::left)
		.def_readwrite("top",					&RECT::top)
		.def_readwrite("bottom",				&RECT::bottom),

		class_<CUIWindow>("CUIWindow")
		.def(					constructor<>())
		.def("AttachChild",  &CUIWindow::AttachChild, adopt(_2))
		.def("DetachChild",  &CUIWindow::DetachChild)
		.def("SetWndRect",  (void (CUIWindow::*)(RECT))				 CUIWindow::SetWndRect)
		.def("SetWndRect",  (void (CUIWindow::*)(int,int,int,int))   CUIWindow::SetWndRect)
		.def("SetAutoDelete",  &CUIWindow::SetAutoDelete)
		.def("IsAutoDelete",  &CUIWindow::IsAutoDelete)
		.def("GetWidth",  &CUIWindow::GetWidth)
		.def("SetWidth",  &CUIWindow::SetWidth)
		.def("GetHeight",  &CUIWindow::GetHeight)
		.def("SetHeight",  &CUIWindow::SetHeight),

		class_<CUIDialogWnd, CUIWindow>("CUIDialogWnd"),

		class_<CUIStatsWnd, CUIDialogWnd>("CUIStatsWnd")
		.def("GetFrameRect",  &CUIStatsWnd::GetFrameRect),

		class_<CUIDMFragList, CUIStatsWnd>("CUIDMFragList")
		.def(					constructor<>()),

		class_<CUIDMPlayerList, CUIDMFragList>("CUIDMPlayerList")
		.def(					constructor<>()),

		class_<CUIInventoryWnd, CUIDialogWnd>("CUIInventoryWnd")
		.def(					constructor<>()),

		class_<CGameFont>("CGameFont")
			.enum_("EAligment")
			[
				value("alLeft",						int(CGameFont::alLeft)),
				value("alRight",					int(CGameFont::alRight)),
				value("alCenter",					int(CGameFont::alCenter))
			],

		class_<CUICaption>("CUICaption")
		.def("addCustomMessage",	&CUICaption::addCustomMessage)
		.def("setCaption",			&CUICaption::setCaption)
	];
}
