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

	hGeom.create(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
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

void CUIStaticItem::SetShader(const ref_shader& sh)
{
//	hShader.destroy();
	hShader = sh;
}

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
	u32				vOffset;
	int				v_cnt			= 0;
	FVF::TL*		pv				= (FVF::TL*)RCache.Vertex.Lock	(4*(iTileX+1)*(iTileY+1),hGeom.stride(),vOffset);

	Ivector2		pos;
	float fw			= iVisRect.width()  * UI()->GetScaleX();
	float fh			= iVisRect.height() * UI()->GetScaleY();

	int				x,y;
	for (x=0; x<iTileX; ++x){
		for (y=0; y<iTileY; ++y){
			pos.set					(iCeil(bp.x+x*fw),iCeil(bp.y+y*fh));
			inherited::Render		(pv,pos,dwColor);
			v_cnt	+=4;
		}
	}
	Irect clip_rect	= {bp.x,bp.y,iFloor(bp.x+fw*iTileX+iRemX),iFloor(bp.y+fh*iTileY+iRemY)};
	UI()->PushScissor(clip_rect);
	if (iRemX){
		for (y=0; y<iTileY; ++y){
			pos.set					(iCeil(bp.x+iTileX*fw),iCeil(bp.y+y*fh));
			inherited::Render		(pv,pos,dwColor);	
			v_cnt	+=4;
		}
	}
	if (iRemY){
		for (x=0; x<iTileX; ++x){
			pos.set					(iCeil(bp.x+x*fw),iCeil(bp.y+iTileY*fh));
			inherited::Render		(pv,pos,dwColor,iVisRect.x1,iVisRect.y1,iVisRect.x2,iRemY);	
			v_cnt	+=4;
		}
	}
	if (iRemX&&iRemY){
		pos.set						(iCeil(bp.x+iTileX*fw),iCeil(bp.y+iTileY*fh));
		inherited::Render			(pv,pos,dwColor,iVisRect.x1,iVisRect.y1,iRemX,iRemY);	
		v_cnt		+=4;
	}
	UI()->PopScissor();

	// unlock VB and Render it as triangle LIST
	RCache.Vertex.Unlock	(v_cnt,hGeom.stride());
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,v_cnt,0,v_cnt/2);
}
//--------------------------------------------------------------------
void CUIStaticItem::Render(float angle, const ref_shader& sh)
{
	VERIFY(g_bRendering);
	// установить обязательно перед вызовом CustomItem::Render() !!!
	VERIFY(sh?sh:hShader);
	RCache.set_Shader			(sh?sh:hShader);
	// convert&set pos
	Ivector2		bp;
	UI()->ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);

	// actual rendering
	u32			vOffset;
	FVF::TL*		pv				= (FVF::TL*)RCache.Vertex.Lock	(4,hGeom.stride(),vOffset);
	 
	inherited::Render(pv,bp,dwColor,angle);

	// unlock VB and Render it as triangle LIST
	RCache.Vertex.Unlock	(4,hGeom.stride());
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4,0,2);
}
//--------------------------------------------------------------------
//используется для вывода растянутой и повернутой 
//части текстуры в том прямоугольнике,
//который задается VisRect

void CUIStaticItem::Render(float x1, float y1, float x2, float y2, 
						   float x3, float y3, float x4, float y4, const ref_shader& sh)
{
	VERIFY(g_bRendering);
	// установить обязательно перед вызовом CustomItem::Render() !!!
	VERIFY(sh?sh:hShader);
	RCache.set_Shader		(sh?sh:hShader);
	// convert&set pos
	Ivector2		bp;
	UI()->ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);

	// actual rendering
	u32			vOffset;
	FVF::TL*		pv				= (FVF::TL*)RCache.Vertex.Lock	(4,hGeom.stride(),vOffset);
	
	inherited::RenderTexPart(pv,bp,dwColor,x1,y1,x2,y2,x3,y3,x4,y4);	
	
	// unlock VB and Render it as triangle LIST
	RCache.Vertex.Unlock	(4,hGeom.stride());
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4,0,2);

}
//вывод изображения из OriginalRect на текстуре в заданную область экрана 
//при этом меняется VisRect
void CUIStaticItem::Render(int x1, int y1, int x2, int y2, 
							const ref_shader& sh)
{
	SetRect(x1, y1, x2, y2); 
	Render(0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, sh);
}

