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

	hVS			= Device.Shader._CreateVS	(FVF::F_TL);
	hShader		= Device.Shader.Create		("hud\\cursor","ui\\ui_cursor",FALSE);
}
//--------------------------------------------------------------------
CUICursor::~CUICursor()
{
	Device.Shader.Delete	(hShader);
	Device.Shader._DeleteVS	(hVS);
}
//--------------------------------------------------------------------
void CUICursor::Render()
{
	_VertexStream*	Stream	= &Device.Streams.Vertex; 
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,hVS->dwStride,vOffset);
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
	Stream->Unlock					(4,hVS->dwStride);
	Device.Shader.set_Shader		(hShader);

	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Stream->Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
	UPDATEC							(4,2,1);
}
//--------------------------------------------------------------------
