// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:58 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uicursor.h"

#include "..\CustomHUD.h"

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICursor::CUICursor()
{    
	vPos.set		(0.f,0.f);

	Stream		= Device.Streams.Create	(FVF::F_TL,4);
	hShader		= Device.Shader.Create	("hud\\cursor","ui\\ui_cursor",FALSE);
}
//--------------------------------------------------------------------

CUICursor::~CUICursor()
{
	Device.Shader.Delete	(hShader);
}
//--------------------------------------------------------------------
void CUICursor::Render()
{
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,vOffset);
	float			size= 2 * Device.dwWidth * 0.015f;
	
	// Convert to screen coords
	float cx        = Device._x2real(vPos.x);
	float cy        = Device._y2real(vPos.y);
	
	DWORD C			= C_DEFAULT;

	pv->set(cx, cy+size,		.0001f,.9999f, C, 0, 1); pv++;
	pv->set(cx, cy,				.0001f,.9999f, C, 0, 0); pv++;
	pv->set(cx+size, cy+size,	.0001f,.9999f, C, 1, 1); pv++;
	pv->set(cx+size, cy,		.0001f,.9999f, C, 1, 0); pv++;
	
	// unlock VB and Render it as triangle list
	Stream->Unlock			(4);
	Device.Shader.set_Shader(hShader);
	Device.Primitive.Draw	(Stream,4,2,vOffset,Device.Streams_QuadIB);
}
//--------------------------------------------------------------------
