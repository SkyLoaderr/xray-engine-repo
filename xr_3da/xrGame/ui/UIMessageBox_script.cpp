#include "stdafx.h"
#include "UIMessageBox.h"
#include "../script_space.h"

using namespace luabind;

void CUIMessageBox::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIMessageBox,CUIFrameWindow>("CUIMessageBox")
		.def(					constructor<>())
		.enum_("style")
		[
			value("STYLE_OK",					int(CUIMessageBox::MESSAGEBOX_OK)),
			value("STYLE_YES_NO",				int(CUIMessageBox::MESSAGEBOX_YES_NO)),
			value("STYLE_YES_NO_CANCEL",		int(CUIMessageBox::MESSAGEBOX_YES_NO_CANCEL))
		]
		.def("SetText",					(void(CUIMessageBox::*)(LPCSTR))CUIMessageBox::SetText)
		.def("Show",					(void(CUIMessageBox::*)(void))CUIMessageBox::Show)
		.def("Hide",					&CUIMessageBox::Hide)
		.def("AutoCenter",				&CUIMessageBox::AutoCenter)
		.def("SetStyle",				&CUIMessageBox::SetStyle_script)
//		.def("",					&CUIMessageBox::)

	];
}