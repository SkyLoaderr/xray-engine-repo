// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:36 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uistaticitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIStaticItem::CUIStaticItem()
{    
	dwColor		= 0xffffffff;
	hShader		= 0;
}
//--------------------------------------------------------------------

CUIStaticItem::~CUIStaticItem()
{
	Device.Shader._DeleteVS (hVS);
	Device.Shader.Delete	(hShader);
}
//--------------------------------------------------------------------

void CUIStaticItem::Init(LPCSTR tex, LPCSTR sh, float left, float top, float tx_width, float tx_height, DWORD align)
{
	Init			(left,top,tx_width,tx_height,align);
	if (0==hShader)	hShader	= Device.Shader.Create		(sh,tex);
	if (0==hVS)		hVS		= Device.Shader._CreateVS	(FVF::F_TL);
}

void CUIStaticItem::Init(float left, float top, float tx_width, float tx_height, DWORD align)
{
	inherited::Init						(tx_width,tx_height);
	Level().HUD()->ClientToScreenScaled	(left,top,vPos,align);
}

void CUIStaticItem::Render(Shader* sh)
{
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv				= (FVF::TL*)Device.Streams.Vertex.Lock	(4,hVS->dwStride,vOffset);
	
	inherited::Render				(pv,vPos,dwColor);	

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(4,hVS->dwStride);
	Device.Shader.set_Shader		(sh?sh:hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
	UPDATEC							(4,2,1);
}
//--------------------------------------------------------------------

void CUIStaticItem::Render(float angle, Shader* sh)
{
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv				= (FVF::TL*)Device.Streams.Vertex.Lock	(4,hVS->dwStride,vOffset);
	
	inherited::Render(pv,vPos,dwColor,angle);	

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(4,hVS->dwStride);
	Device.Shader.set_Shader		(sh?sh:hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
	UPDATEC							(4,2,1);
}
//--------------------------------------------------------------------
