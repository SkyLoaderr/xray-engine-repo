#include "stdafx.h"
#pragma hdrstop

#include "XR_HUDFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFontHUD::CFontHUD()
{
	pShader = Device.Shader.Create("font","fonts\\font_hud",false);

	TextureSize.x	= 128;
	TextureSize.y	= 128;
	iNumber			= 16;
	bGradient		= false;

	// Character map
	for (int i=0; i<256; i++) CharMap[i]=i;
//	CharMap[' ']=-1;

	// Default params
	Color(0xFFFFFFFF);
	Size (1.f);
	Interval(0.75f,1.f);
}
