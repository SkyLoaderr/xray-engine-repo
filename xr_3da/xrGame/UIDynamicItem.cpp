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
	hShader		= 0;
	hVS			= 0;
}
//--------------------------------------------------------------------
CUIDynamicItem::~CUIDynamicItem()
{
	Device.Shader.Delete	(hShader);
	Device.Shader._DeleteVS	(hVS);
}

//--------------------------------------------------------------------
void CUIDynamicItem::Init	(LPCSTR tex, LPCSTR sh)
{
	if (0==hVS)		hVS		= Device.Shader._CreateVS	(FVF::F_TL);
	if (0==hShader)	hShader	= Device.Shader.Create		(sh,tex,FALSE);
}

void CUIDynamicItem::Out(int left, int top, u32 color, u32 align)
{
	SDynamicItemData* D = 0;
	if (data.size()<=item_cnt){ 
		data.push_back(SDynamicItemData());
		D = &data.back();
	}else{
		D = &data[item_cnt];
	}
	Level().HUD()->ClientToScreenScaled(D->pos,left,top,align);
	D->color = color;
	item_cnt++;
}


void CUIDynamicItem::Render	()
{
	if (!item_cnt)	return;

	// actual rendering
	u32			vOffset;
	FVF::TL*		pv	= (FVF::TL*) Device.Streams.Vertex.Lock(item_cnt*4,hVS->dwStride,vOffset);
	
	DIDIt it		= data.begin();
	for (u32 i=0; i<item_cnt; i++,it++)
		inherited::Render(pv,it->pos,it->color);

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(item_cnt*4,hVS->dwStride);
	Device.Shader.set_Shader		(hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,item_cnt*4,0,item_cnt*2);
}

//--------------------------------------------------------------------
void CUIDynamicItem::Render(float angle)
{
	if (!item_cnt) return;

	// actual rendering
	u32			vOffset;
	FVF::TL*		pv	= (FVF::TL*) Device.Streams.Vertex.Lock(item_cnt*4,hVS->dwStride,vOffset);
	
	DIDIt it		= data.begin();
	for (u32 i=0; i<item_cnt; i++,it++)
		inherited::Render(pv,it->pos,it->color,angle);	

	// unlock VB and Render it as triangle list
	Device.Streams.Vertex.Unlock	(item_cnt*4,hVS->dwStride);
	Device.Shader.set_Shader		(hShader);
	Device.Primitive.setVertices	(hVS->dwHandle,hVS->dwStride,Device.Streams.Vertex.Buffer());
	Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);
	Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,item_cnt*4,0,item_cnt*2);
}
//--------------------------------------------------------------------
