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
	Device.Shader.Delete(hShader);
}
//--------------------------------------------------------------------

void CUIDynamicItem::Init(LPCSTR tex, LPCSTR sh, float tx_width, float tx_height){
	inherited::Init(tx_width,tx_height);
	Stream		= Device.Streams.Create	(FVF::F_TL,128);
	hShader		= Device.Shader.Create	(sh,tex,FALSE);
}
 
void CUIDynamicItem::Out(float left, float top, DWORD color, DWORD align){
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


void CUIDynamicItem::Render()
{
	if (!item_cnt) return;
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(item_cnt*4,vOffset);
	
	DIDIt it		= data.begin();
	for (DWORD i=0; i<item_cnt; i++,it++)
		inherited::Render(pv,it->pos,it->color);	

	// unlock VB and Render it as triangle list
	Stream->Unlock			(item_cnt*4);
	Device.Shader.Set		(hShader);
	Device.Primitive.Draw	(Stream,item_cnt*4,item_cnt*2,vOffset,Device.Streams_QuadIB);
}
//--------------------------------------------------------------------

void CUIDynamicItem::Render(float angle){
	if (!item_cnt) return;
	// actual rendering
	DWORD			vOffset;
	FVF::TL*		pv	= (FVF::TL*)Stream->Lock(item_cnt*4,vOffset);
	
	DIDIt it		= data.begin();
	for (DWORD i=0; i<item_cnt; i++,it++)
		inherited::Render(pv,it->pos,it->color,angle);	

	// unlock VB and Render it as triangle list
	Stream->Unlock			(item_cnt*4);
	Device.Shader.Set		(hShader);
	Device.Primitive.Draw	(Stream,item_cnt*4,item_cnt*2,vOffset,Device.Streams_QuadIB);
}
//--------------------------------------------------------------------
