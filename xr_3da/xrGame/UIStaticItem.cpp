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
	hVS			= 0;
	iTileX		= 1;
	iTileY		= 1;
	iRemX		= 0;
	iRemY		= 0;
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
	inherited::Init	(tx_width,tx_height);
	Level().HUD()->ClientToScreenScaled	(vPos,left,top,align);
	if (0==hVS)		hVS		= Device.Shader._CreateVS	(FVF::F_TL);
}

void CUIStaticItem::Render(Shader* sh)
{
	// actual rendering
	DWORD			vOffset;
	int				v_cnt			= 0;
	FVF::TL*		pv				= (FVF::TL*)Device.Streams.Vertex.Lock	(4*(iTileX+1)*(iTileY+1),hVS->dwStride,vOffset);

	Fvector2		pos;
	int fw			= Rect.rb.x-Rect.lt.x;
	int fh			= Rect.rb.y-Rect.lt.y;
	int				x,y;
	for (x=0; x<iTileX; x++){
		for (y=0; y<iTileY; y++){
			pos.set					(vPos.x+x*fw,vPos.y+y*fh);
			inherited::Render		(pv,pos,dwColor);
			v_cnt	+=4;
		}
	}
	if (iRemX){
		for (y=0; y<iTileY; y++){
			pos.set					(vPos.x+iTileX*fw,vPos.y+y*fh);
			inherited::Render		(pv,pos,dwColor,Rect.x1,Rect.y1,iRemX,Rect.y2);	
			v_cnt	+=4;
		}
	}
	if (iRemY){
		for (x=0; x<iTileX; x++){
			pos.set					(vPos.x+x*fw,vPos.y+iTileY*fh);
			inherited::Render		(pv,pos,dwColor,Rect.x1,Rect.y1,Rect.x2,iRemY);	
			v_cnt	+=4;
		}
	}
	if (iRemX&&iRemY){
		pos.set						(vPos.x+iTileX*fw,vPos.y+iTileY*fh);
		inherited::Render			(pv,pos,dwColor,Rect.x1,Rect.y1,iRemX,iRemY);	
		v_cnt		+=4;
	}

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(v_cnt,hVS->dwStride);
	Device.Shader.set_Shader		(sh?sh:hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,v_cnt,0,v_cnt/2);
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
}
//--------------------------------------------------------------------
