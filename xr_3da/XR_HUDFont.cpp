// XR_CustomFont.cpp: implementation of the CFontBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "XR_HUDFont.h"
#include "hw.h"
#include "xr_creator.h"

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
