#include "stdafx.h"
#include "UIStatic.h"
#include "../script_space.h"

using namespace luabind;

void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIStatic, CUIWindow>("UIStatic")
		.def(						constructor<>())

		.def("SetText",				(void (CUIStatic::*)(LPCSTR)) (&CUIStatic::SetText) )
		.def("GetText",				&CUIStatic::GetText)

		.def("SetColor",			&CUIStatic::SetColor)
		.def("GetColor",			&CUIStatic::GetColor)

		.def("InitTexture",			&CUIStatic::InitTexture)
		.def("SetOriginalRect",		&CUIStatic::SetOriginalRect)

		.def("SetTextureScale",		&CUIStatic::SetTextureScale)
		.def("GetTextureScale",		&CUIStatic::GetTextureScale)

		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetStretchTexture",	&CUIStatic::GetStretchTexture)

		.def("SetTextAlign",		&CUIStatic::SetTextAlign_script)
		.def("GetTextAlign",		&CUIStatic::GetTextAlign_script)

/*	
		.def("",		&CUIStatic::)
		.def("",		&CUIStatic::)
		.def("",		&CUIStatic::)
*/		
		
		
	];
}