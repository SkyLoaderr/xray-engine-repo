#include "stdafx.h"
#pragma hdrstop

#include "XR_SmallFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFontSmall::CFontSmall()
{
//	FILE_NAME tex_name;
//	pSettings->ReadString	( "interface", "font_small", tex_name );
	pShader = Device.Shader.Create("font","fonts\\small",false);

	TextureSize.x	= 256;
	TextureSize.y	= 256;
	iNumber			= 16;

	// Character map
	for (int i=0; i<256; i++) CharMap[i]=i;
	CharMap[' ']=-1;

	// Default params
	Color(D3DCOLOR_RGBA(0x00,0x00,0xff,0xff));
	Size (0.025f);
	Interval(0.65f,1.f);
}
