#include "stdafx.h"
#include "UIStatic.h"
#include "../script_space.h"

using namespace luabind;

void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIStatic, CUIWindow>("CUIStatic")
		.def(						constructor<>())

		.def("SetText",				(void (CUIStatic::*)(LPCSTR)) (&CUIStatic::SetText) )
		.def("SetText",				(void (CUIStatic::*)(LPCSTR)) (&CUIStatic::SetText) )
		.def("GetText",				&CUIStatic::GetText)

		.def("SetTextX",				&CUIStatic::SetTextX)
		.def("SetTextY",				&CUIStatic::SetTextY)
		.def("GetTextX",				&CUIStatic::GetTextX)
		.def("GetTextY",				&CUIStatic::GetTextY)
		
		.def("SetColor",			&CUIStatic::SetColor)
		.def("GetColor",			&CUIStatic::GetColor)
		.def("Init",				(void(CUIStatic::*)(int,int,int,int))CUIStatic::Init )
		.def("Init",				(void(CUIStatic::*)(LPCSTR,int,int,int,int))CUIStatic::Init )
		.def("InitTexture",			&CUIStatic::InitTexture )
		.def("SetTextureOffset",	&CUIStatic::SetTextureOffset )


		.def("SetOriginalRect",		&CUIStatic::SetOriginalRect)

		.def("SetTextureScale",		&CUIStatic::SetTextureScale)
		.def("GetTextureScale",		&CUIStatic::GetTextureScale)

		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetStretchTexture",	&CUIStatic::GetStretchTexture)

		.def("SetTextAlign",		&CUIStatic::SetTextAlign_script)
		.def("GetTextAlign",		&CUIStatic::GetTextAlign_script)

	
		.def("ClipperOn",			&CUIStatic::ClipperOn)
		.def("ClipperOff",			(void(CUIStatic::*)(void))CUIStatic::ClipperOff )
		.def("GetClipperState",		&CUIStatic::GetClipperState)
//		.def("",		&CUIStatic::)
//		.def("",		&CUIStatic::)
//		.def("",		&CUIStatic::)
//		.def("",		&CUIStatic::)
//		.def("",		&CUIStatic::)
	];
}