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

void CUIStaticItem::Init(LPCSTR tex, LPCSTR sh, int left, int top, int tx_width, int tx_height, DWORD align)
{
	Init			(left,top,tx_width,tx_height,align);
	if (0==hShader)	hShader	= Device.Shader.Create		(sh,tex);
	if (0==hVS)		hVS		= Device.Shader._CreateVS	(FVF::F_TL);
}

void CUIStaticItem::Init(int left, int top, int tx_width, int tx_height, DWORD align)
{
	inherited::Init	(tx_width,tx_height);
	Level().HUD()->ClientToScreenScaled	(iPos,left,top,align);
	if (0==hVS)		hVS		= Device.Shader._CreateVS	(FVF::F_TL);
}

void CUIStaticItem::Render(Shader* sh)
{
	// actual rendering
	DWORD			vOffset;
	int				v_cnt			= 0;
	FVF::TL*		pv				= (FVF::TL*)Device.Streams.Vertex.Lock	(4*(iTileX+1)*(iTileY+1),hVS->dwStride,vOffset);

	Ivector2		pos;
	int fw			= iRect.width();
	int fh			= iRect.height();
	int				x,y;
	for (x=0; x<iTileX; x++){
		for (y=0; y<iTileY; y++){
			pos.set					(iPos.x+x*fw,iPos.y+y*fh);
			inherited::Render		(pv,pos,dwColor);
			v_cnt	+=4;
		}
	}
	if (iRemX){
		for (y=0; y<iTileY; y++){
			pos.set					(iPos.x+iTileX*fw,iPos.y+y*fh);
			inherited::Render		(pv,pos,dwColor,iRect.x1,iRect.y1,iRemX,iRect.y2);	
			v_cnt	+=4;
		}
	}
	if (iRemY){
		for (x=0; x<iTileX; x++){
			pos.set					(iPos.x+x*fw,iPos.y+iTileY*fh);
			inherited::Render		(pv,pos,dwColor,iRect.x1,iRect.y1,iRect.x2,iRemY);	
			v_cnt	+=4;
		}
	}
	if (iRemX&&iRemY){
		pos.set						(iPos.x+iTileX*fw,iPos.y+iTileY*fh);
		inherited::Render			(pv,pos,dwColor,iRect.x1,iRect.y1,iRemX,iRemY);	
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
	
	inherited::Render(pv,iPos,dwColor,angle);	

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(4,hVS->dwStride);
	Device.Shader.set_Shader		(sh?sh:hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4,0,2);
}
//--------------------------------------------------------------------
