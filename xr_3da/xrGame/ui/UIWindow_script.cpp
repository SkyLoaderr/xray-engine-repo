#include "stdafx.h"
#include "UIWindow.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIDialogWnd.h"

#include "../script_space.h"
#include <luabind\adopt_policy.hpp>


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

		class_<CUIWindow>("CUIWindow")
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
		.def("SetHeight",				&CUIWindow::SetHeight)

		.def("Enable",					&CUIWindow::Enable)
		.def("IsEnabled",				&CUIWindow::IsEnabled)
		.def("Show",					&CUIWindow::Show)
		.def("IsShown",					&CUIWindow::IsShown)

		.def("WindowName",				&CUIWindow::WindowName_script)
		.def("SetWindowName",			&CUIWindow::SetWindowName),
//		.def("",						&CUIWindow::)
		
		
		class_<CUIDialogWnd, CUIWindow>("CUIDialogWnd"),

		class_<CUIFrameWindow, CUIWindow>("CUIFrameWindow")
		.def(					constructor<>())
		.def("SetWidth",				&CUIFrameWindow::SetWidth)
		.def("SetHeight",				&CUIFrameWindow::SetHeight)
		.def("SetColor",				&CUIFrameWindow::SetColor)
		.def("GetTitleStatic",			&CUIFrameWindow::GetTitleStatic),
//		.def("",						&CUIFrameWindow::)

		class_<CUIFrameLineWnd, CUIWindow>("CUIFrameLineWnd")
		.def(					constructor<>())

		.def("SetWidth",						&CUIFrameLineWnd::SetWidth)
		.def("SetHeight",						&CUIFrameLineWnd::SetHeight)
		.def("SetOrientation",					&CUIFrameLineWnd::SetOrientation)
		.def("SetColor",						&CUIFrameLineWnd::SetColor)
		.def("GetTitleStatic",					&CUIFrameLineWnd::GetTitleStatic)
//		.def("",						&CUIFrameLineWnd::)
//		.def("",						&CUIFrameLineWnd::)
//		.def("",						&CUIFrameLineWnd::)

	];
}