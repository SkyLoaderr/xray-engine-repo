#include "stdafx.h"
#include "x_ray.h"
#include "XR_GameFont.h"
#include "XR_ini.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFontGame::CFontGame()
{
//	FILE_NAME tex_name;
//	pSettings->ReadString	( "interface", "font_game", tex_name );
	pShader = Device.Shader.Create("font","fonts\\game",false);

	TextureSize.x	= 256;
	TextureSize.y	= 256;
	iNumber			= 8;

	// Character map
	int i;
	for (i=0; i<256; CharMap[i++]=-1);
	for (i=0; i<26;  i++) CharMap[i+'A']=i;
	for (i=0; i<26;  i++) CharMap[i+'a']=i+26;
	for (i=0; i<10;  i++) CharMap[i+'0']=i+52;
	CharMap[':']=62;
	CharMap['!']=63;

	// Default params
	Color(D3DCOLOR_RGBA(0x66,0xFB,0x23,0xff));
	Size (0.05f);
	Interval(0.65f,1.f);
}
