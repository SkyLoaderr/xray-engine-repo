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
//		.def("SetLines",			&CUIStatic::SetLines)
		.def("GetText",				&CUIStatic::GetText)

		.def("SetTextX",				&CUIStatic::SetTextX)
		.def("SetTextY",				&CUIStatic::SetTextY)
		.def("GetTextX",				&CUIStatic::GetTextX)
		.def("GetTextY",				&CUIStatic::GetTextY)
		
		.def("SetColor",			&CUIStatic::SetColor)
		.def("GetColor",			&CUIStatic::GetColor)
		.def("SetTextColor",		&CUIStatic::SetTextColor_script)
		.def("Init",				(void(CUIStatic::*)(float,float,float,float))CUIStatic::Init )
		.def("Init",				(void(CUIStatic::*)(LPCSTR,float,float,float,float))CUIStatic::Init )
		.def("InitTexture",			&CUIStatic::InitTexture )
//		.def("InitSharedTexture",	&CUIStatic::InitSharedTexture)
		.def("SetTextureOffset",	&CUIStatic::SetTextureOffset )


		.def("SetOriginalRect",		(void(CUIStatic::*)(float,float,float,float))&CUIStatic::SetOriginalRect)
/*
		.def("SetTextureScaleXY",	&CUIStatic::SetTextureScaleXY)
		.def("GetTextureScaleX",	&CUIStatic::GetTextureScaleX)
		.def("GetTextureScaleY",	&CUIStatic::GetTextureScaleY)
*/
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