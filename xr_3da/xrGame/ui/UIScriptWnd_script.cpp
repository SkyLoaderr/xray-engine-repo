#include "stdafx.h"
#include "UIScriptWnd.h"
#include "../script_space.h"


using namespace luabind;

void UIScriptWnd::script_register(lua_State *L)
{
	module(L)
	[
		class_<UIScriptWnd>("UIScriptWnd")
		.def(					constructor<>())

		.def("Load",			&UIScriptWnd::Load)
		.def("AddCallback",		(void (UIScriptWnd::*)(const luabind::functor<void>&)) UIScriptWnd::AddCallback)
		.def("AddCallback",		(void (UIScriptWnd::*)(const luabind::object&, LPCSTR)) UIScriptWnd::AddCallback)

		.def("test",			&UIScriptWnd::test)

	];
}