// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:36 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uistaticitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIStaticItem::CUIStaticItem()
{    
	dwColor			= 0xffffffff;
	iTileX			= 1;
	iTileY			= 1;
	iRemX			= 0;
	iRemY			= 0;

	hShader			= NULL;

	hGeom_list.create	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	hGeom_fan.create	(FVF::F_TL, RCache.Vertex.Buffer(), 0);
}
//--------------------------------------------------------------------
CUIStaticItem::~CUIStaticItem()
{
}
//--------------------------------------------------------------------

void CUIStaticItem::CreateShader(LPCSTR tex, LPCSTR sh)
{
	hShader.create	(sh,tex);
}
//--------------------------------------------------------------------

void CUIStaticItem::SetShader(const ref_shader& sh)
{
//	hShader.destroy();
	hShader = sh;
}
//--------------------------------------------------------------------

void CUIStaticItem::Init(LPCSTR tex, LPCSTR sh, int left, int top, u32 align)
{
	//by Dandy
	//used for reinitializing StaticItems during the game
//	hShader.destroy();

	uFlags &= !flValidRect;

	CreateShader	(tex,sh);
	SetPos			(left,top);
	SetAlign		(align);
}
//--------------------------------------------------------------------

void CUIStaticItem::Render		(const ref_shader& sh)
{
	VERIFY(g_bRendering);
	// установить обязательно перед вызовом CustomItem::Render() !!!
	VERIFY(sh?sh:hShader);
	RCache.set_Shader			(sh?sh:hShader);
	// convert&set pos
	Ivector2		bp;
	UI()->ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);

	// actual rendering
	u32							vOffset;
	Ivector2					pos;
	float fw					= iVisRect.x2*UI()->GetScaleX();
	float fh					= iVisRect.y2*UI()->GetScaleY();

	int tile_x					= (iRemX==0)?iTileX:iTileX+1;
	int tile_y					= (iRemY==0)?iTileY:iTileY+1;
	int							x,y;
	// set scissor
	Irect clip_rect				= {iPos.x,iPos.y,iFloor(iPos.x+iVisRect.x2*iTileX+iRemX),iFloor(iPos.y+iVisRect.y2*iTileY+iRemY)};
	UI()->PushScissor			(clip_rect);
	// set geom
	RCache.set_Geometry			(hGeom_fan);
	// render
	for (x=0; x<tile_x; ++x){
		for (y=0; y<tile_y; ++y){
			pos.set				(iCeil(bp.x+x*fw),iCeil(bp.y+y*fh));
			FVF::TL* start_pv	= (FVF::TL*)RCache.Vertex.Lock	(32,hGeom_fan.stride(),vOffset);
			FVF::TL* pv			= start_pv;
			inherited::Render	(pv,pos,dwColor);
			u32 p_cnt			= pv-start_pv;
			RCache.Vertex.Unlock(p_cnt,hGeom_fan.stride());
			if (p_cnt>2) RCache.Render	(D3DPT_TRIANGLEFAN,vOffset,p_cnt-2);
		}
	}
	UI()->PopScissor();
}
//--------------------------------------------------------------------

void CUIStaticItem::Render(float angle, const ref_shader& sh)
{
	VERIFY(g_bRendering);
	// установить обязательно перед вызовом CustomItem::Render() !!!
	VERIFY(sh?sh:hShader);
	RCache.set_Shader			(sh?sh:hShader);
	// convert&set pos
	Ivector2 bp;
	UI()->ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);
	// actual rendering
	u32		vOffset;
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	(32,hGeom_fan.stride(),vOffset);
	FVF::TL* pv					= start_pv;
	inherited::Render			(pv,bp,dwColor,angle);
	// unlock VB and Render it as triangle LIST
	u32 p_cnt					= pv-start_pv;
	RCache.Vertex.Unlock		(p_cnt,hGeom_fan.stride());
	RCache.set_Geometry	 		(hGeom_fan);
	if (p_cnt>2) RCache.Render	(D3DPT_TRIANGLEFAN,vOffset,p_cnt-2);
}
//--------------------------------------------------------------------
