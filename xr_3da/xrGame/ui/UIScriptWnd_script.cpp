#include "stdafx.h"
#include "UIScriptWnd.h"
#include "../script_space.h"

//UI-controls
#include "UIButton.h"

using namespace luabind;

void UIScriptWnd::script_register(lua_State *L)
{
	module(L)
	[
		class_<UIScriptWnd>("UIScriptWnd")
		.def(					constructor<>())

		.def("Load",			&UIScriptWnd::Load)
		.def("AddCallback",		&UIScriptWnd::AddCallback)

		.def("test",			&UIScriptWnd::test)

		.def("GetButton",		(CUIButton* (UIScriptWnd::*)(LPCSTR)) UIScriptWnd::GetControl<CUIButton>)
		
	];
}