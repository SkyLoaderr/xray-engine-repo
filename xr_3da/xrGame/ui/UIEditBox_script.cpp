#include "stdafx.h"
#include "UIEditBox.h"

#include "../script_space.h"

using namespace luabind;

void CUIEditBox::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIEditBox, CUIStatic>("UIEditBox")
		.def(						constructor<>())

//		.def("",				&CUIEditBox:: )
	];
}