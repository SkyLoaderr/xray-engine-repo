#include "stdafx.h"
#include "UIWindow.h"
#include "../script_space.h"


using namespace luabind;

void CUIWindow::script_register(lua_State *L)
{
	module(L)
	[
		class_<RECT>("RECT")
		.def(					constructor<>())
		.def_readwrite("right",					&RECT::right)
		.def_readwrite("left",					&RECT::left)
		.def_readwrite("top",					&RECT::top)
		.def_readwrite("bottom",				&RECT::bottom),

		class_<CUIWindow>("UIWindow")
		.def(							constructor<>())
		.def("AttachChild",				&CUIWindow::AttachChild, adopt(_2))
		.def("DetachChild",				&CUIWindow::DetachChild)
		.def("SetAutoDelete",			&CUIWindow::SetAutoDelete)
		.def("IsAutoDelete",			&CUIWindow::IsAutoDelete)

		.def("SetWndRect",				(void (CUIWindow::*)(RECT))				 CUIWindow::SetWndRect)
		.def("SetWndRect",				(void (CUIWindow::*)(int,int,int,int))   CUIWindow::SetWndRect)
		.def("GetWidth",				&CUIWindow::GetWidth)
		.def("SetWidth",				&CUIWindow::SetWidth)
		.def("GetHeight",				&CUIWindow::GetHeight)
		.def("SetHeight",				&CUIWindow::SetHeight),

		.def("Enable",					&CUIWindow::Enable)
		.def("IsEnabled",				&CUIWindow::IsEnabled)
		.def("Show",					&CUIWindow::Show)
		.def("IsShown",					&CUIWindow::IsShown)

		.def("WindowName",				&CUIWindow::WindowName_script)

/*		.def("",						&CUIWindow::)
		.def("",						&CUIWindow::)
*/		
		
	];
}