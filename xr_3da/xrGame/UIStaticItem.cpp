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
	if (hShader) Device.Shader.Delete(hShader);
}
//--------------------------------------------------------------------

void CUIStaticItem::Init(LPCSTR tex, LPCSTR sh, float left, float top, float tx_width, float tx_height, DWORD align){
	Init		(left,top,tx_width,tx_height,align);
	hShader		= Device.Shader.Create	(sh,tex,FALSE);
}
 
void CUIStaticItem::Init(float left, float top, float tx_width, float tx_height, DWORD align){
	inherited::Init(tx_width,tx_height);
	Level().HUD()->ClientToScreenScaled(left,top,vPos,align);
	Stream		= Device.Streams.Create	(FVF::F_TL,4);
}

void CUIStaticItem::Render(Shader* sh)
{
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,vOffset);
	
	inherited::Render(pv,vPos,dwColor);	

	// unlock VB and Render it as triangle list
	Stream->Unlock			(4);
	Device.Shader.Set		(sh?sh:hShader);
	Device.Primitive.Draw	(Stream,4,2,vOffset,Device.Streams_QuadIB);
}
//--------------------------------------------------------------------

void CUIStaticItem::Render(float angle, Shader* sh){
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(4,vOffset);
	
	inherited::Render(pv,vPos,dwColor,angle);	

	// unlock VB and Render it as triangle list
	Stream->Unlock			(4);
	Device.Shader.Set		(sh?sh:hShader);
	Device.Primitive.Draw	(Stream,4,2,vOffset,Device.Streams_QuadIB);
}
//--------------------------------------------------------------------
