// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:27 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uidynamicitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIDynamicItem::CUIDynamicItem()
{    
	item_cnt	= 0;
}
//--------------------------------------------------------------------

CUIDynamicItem::~CUIDynamicItem()
{
	Device.Shader.Delete	(hShader);
	Device.Shader._DeleteVS	(hVS);
}
//--------------------------------------------------------------------
void CUIDynamicItem::Init(LPCSTR tex, LPCSTR sh, float tx_width, float tx_height)
{
	inherited::Init(tx_width,tx_height);
	hVS			= Device.Shader._CreateVS	(FVF::F_TL);
	hShader		= Device.Shader.Create		(sh,tex,FALSE);
}
 
void CUIDynamicItem::Out(float left, float top, DWORD color, DWORD align)
{
	SDynamicItemData* D = 0;
	if (data.size()<=item_cnt){ 
		data.push_back(SDynamicItemData());
		D = &data.back();
	}else{
		D = &data[item_cnt];
	}
	Level().HUD()->ClientToScreenScaled(left,top,D->pos,align);
	D->color = color;
	item_cnt++;
}


void CUIDynamicItem::Render	()
{
	if (!item_cnt)	return;

	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*) Device.Streams.Vertex.Lock(item_cnt*4,hVS->dwStride,vOffset);
	
	DIDIt it		= data.begin();
	for (DWORD i=0; i<item_cnt; i++,it++)
		inherited::Render(pv,it->pos,it->color);

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(item_cnt*4,hVS->dwStride);
	Device.Shader.set_Shader		(hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,item_cnt*4,0,item_cnt*2);
	UPDATEC							(item_cnt*4,item_cnt*2,1);
}

//--------------------------------------------------------------------
void CUIDynamicItem::Render(float angle)
{
	if (!item_cnt) return;

	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*) Device.Streams.Vertex.Lock(item_cnt*4,hVS->dwStride,vOffset);
	
	DIDIt it		= data.begin();
	for (DWORD i=0; i<item_cnt; i++,it++)
		inherited::Render(pv,it->pos,it->color,angle);	

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(item_cnt*4,hVS->dwStride);
	Device.Shader.set_Shader		(hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,item_cnt*4,0,item_cnt*2);
	UPDATEC							(item_cnt*4,item_cnt*2,1);
}
//--------------------------------------------------------------------
