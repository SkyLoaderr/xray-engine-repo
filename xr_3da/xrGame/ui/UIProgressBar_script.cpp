#include "stdafx.h"
#include "UIProgressBar.h"

#include "../script_space.h"

using namespace luabind;

void CUIProgressBar::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIProgressBar, CUIWindow>("UIProgressBar")
		.def(						constructor<>())

		.def("SetRange",				&CUIProgressBar::SetRange)
		.def("GetRange_min",			&CUIProgressBar::GetRange_min)
		.def("GetRange_max",			&CUIProgressBar::GetRange_max)
		.def("GetScrollPos",			&CUIProgressBar::GetScrollPos)
		.def("ProgressInc",				&CUIProgressBar::ProgressInc)
		.def("ProgressDec",				&CUIProgressBar::ProgressDec)

		.def("SetProgressTexture",		&CUIProgressBar::SetProgressTexture)
		.def("SetBackgroundTexture",	&CUIProgressBar::SetBackgroundTexture)

//		.def("",				&CUIProgressBar::)
	];
}