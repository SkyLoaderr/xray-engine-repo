#include "stdafx.h"
#include "UIButton.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UIRadioGroup.h"
#include "../script_space.h"
#include <luabind\adopt_policy.hpp>

using namespace luabind;

void CUIButton::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIButton, CUIStatic>("CUIButton")
		.def(							constructor<>())
		.def("Init",				(void(CUIButton::*)(int,int,int,int))CUIButton::Init )
		.def("Init",				(void(CUIButton::*)(LPCSTR,int,int,int,int))CUIButton::Init )
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
		.def("GetCheckedPos",			&CUIRadioGroup::GetCheckedPos)
		.def("SetCheckedPos",			&CUIRadioGroup::SetCheckedPos)


//		.def("",						&CUIRadioGroup::)

		
		
	];
}