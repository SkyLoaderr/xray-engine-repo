#include "stdafx.h"
#include "UIButton.h"

using namespace luabind;

void CUIButton::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIButton, CUIStatic>("UIButton")
		.def(							constructor<>())
		.def("SetHighlightColor",		&CUIButton::SetHighlightColor)
		.def("EnableTextHighlighting",	&CUIButton::EnableTextHighlighting)

		.def("SetPushOffsetX",			&CUIButton::SetPushOffsetX)
		.def("SetPushOffsetY",			&CUIButton::SetPushOffsetY)
		.def("GetPushOffsetX",			&CUIButton::GetPushOffsetX)
		.def("GetPushOffsetY",			&CUIButton::GetPushOffsetY),
//		.def("",						&CUIButton::)

		class_<CUICheckButton, CUIButton>("CUICheckButton")
		.def(							constructor<>())
		.def("GetCheck",				&CUICheckButton::GetCheck)
		.def("SetCheck",				&CUICheckButton::SetCheck)
		.def("ResetCheck",				&CUICheckButton::ResetCheck),
//		.def("",						&CUICheckButton::)

		class_<CUIRadioButton, CUICheckButton>("CUIRadioButton")
		.def(							constructor<>()),
//		.def("",						&CUIRadioButton::)

		class_<CUIRadioGroup, CUIWindow>("CUIRadioGroup")
		.def(							constructor<>())
		.def("AttachChild",				&CUIRadioGroup::AttachChild, adopt(_2))
		.def("DetachChild",				&CUIRadioGroup::DetachChild, adopt(_2))
//		.def("",						&CUIRadioGroup::)

		
		
	];
}