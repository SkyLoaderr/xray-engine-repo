// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:36 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uistaticitem.h"
#include "hudmanager.h"
#include "level.h"

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
	bReverseRemX	= false;
	bReverseRemY	= false;

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
	// установить обязательно перед вызовом CustomItem::Render() !!!
	RCache.set_Shader			(sh?sh:hShader);
	// convert&set pos
	Ivector2		bp;
	HUD().ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);

	// actual rendering
	u32				vOffset;
	int				v_cnt			= 0;
	FVF::TL*		pv				= (FVF::TL*)RCache.Vertex.Lock	(4*(iTileX+1)*(iTileY+1),hGeom.stride(),vOffset);

	Ivector2		pos;
	float fw			= iVisRect.width()*HUD().GetScale();
	float fh			= iVisRect.height()*HUD().GetScale();

	int				x,y;
	for (x=0; x<iTileX; ++x){
		for (y=0; y<iTileY; ++y){
			pos.set					(iCeil(bp.x+x*fw),iCeil(bp.y+y*fh));
			inherited::Render		(pv,pos,dwColor);
			v_cnt	+=4;
		}
	}
	if (iRemX){
		for (y=0; y<iTileY; ++y){
			pos.set					(iCeil(bp.x+iTileX*fw),iCeil(bp.y+y*fh));
//			pos.set					(static_cast<int>(bp.x+iTileX*fw),static_cast<int>(bp.y+y*fh));
			if (bReverseRemX)
			{
				pos.x -= static_cast<int>((iVisRect.x2 - iRemX) * HUD().GetScale() + 0.5f);
				inherited::Render		(pv,pos,dwColor,iVisRect.x2 - iRemX,iVisRect.y1,iVisRect.x2,iVisRect.y2);	
			}
			else
				inherited::Render		(pv,pos,dwColor,iVisRect.x1,iVisRect.y1,iRemX,iVisRect.y2);	
			v_cnt	+=4;
		}
	}
	if (iRemY){
		for (x=0; x<iTileX; ++x){
			pos.set					(iCeil(bp.x+x*fw),iCeil(bp.y+iTileY*fh));
//			pos.set					(static_cast<int>(bp.x+x*fw),static_cast<int>(bp.y+iTileY*fh));
			if (bReverseRemY)
			{
				pos.y -= static_cast<int>((iVisRect.y2 - iRemY) * HUD().GetScale() + 0.5f);
				inherited::Render		(pv,pos,dwColor,iVisRect.x1,iVisRect.y2 - iRemY,iVisRect.x2,iVisRect.y2);	
			}
			else
				inherited::Render		(pv,pos,dwColor,iVisRect.x1,iVisRect.y1,iVisRect.x2,iRemY);	
			v_cnt	+=4;
		}
	}
	if (iRemX&&iRemY){
		pos.set						(iCeil(bp.x+iTileX*fw),iCeil(bp.y+iTileY*fh));
//		pos.set						(static_cast<int>(bp.x+iTileX*fw),static_cast<int>(bp.y+iTileY*fh));
		if (bReverseRemY && bReverseRemX)
		{
			pos.x -= static_cast<int>((iVisRect.x2 - iRemX) * HUD().GetScale() + 0.5f);
			pos.y -= static_cast<int>((iVisRect.y2 - iRemY) * HUD().GetScale() + 0.5f);
			inherited::Render		(pv,pos,dwColor,iVisRect.x2 - iRemX, iVisRect.y2 - iRemY, iVisRect.x2, iVisRect.y2);	
		}
		else if (bReverseRemY)
		{
			pos.y -= static_cast<int>((iVisRect.y2 - iRemY) * HUD().GetScale() + 0.5f);
			inherited::Render		(pv,pos,dwColor,iVisRect.x1,iVisRect.y2 - iRemY, iRemX, iVisRect.y2);
		}
		else if (bReverseRemX)
		{
			pos.x -= static_cast<int>((iVisRect.x2 - iRemX) * HUD().GetScale() + 0.5f);
			inherited::Render		(pv,pos,dwColor,iVisRect.x2 - iRemX,iVisRect.y1,iVisRect.x2,iRemY);	
		}
		else
			inherited::Render		(pv,pos,dwColor,iVisRect.x1,iVisRect.y1,iRemX,iRemY);	
		v_cnt		+=4;
	}

	// unlock VB and Render it as triangle LIST
	RCache.Vertex.Unlock	(v_cnt,hGeom.stride());
	RCache.set_Geometry		(hGeom);
	RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,v_cnt,0,v_cnt/2);
}
//--------------------------------------------------------------------
void CUIStaticItem::Render(float angle, const ref_shader& sh)
{
	// установить обязательно перед вызовом CustomItem::Render() !!!
	RCache.set_Shader			(sh?sh:hShader);
	// convert&set pos
	Ivector2		bp;
	HUD().ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);

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
	// установить обязательно перед вызовом CustomItem::Render() !!!
	RCache.set_Shader		(sh?sh:hShader);
	// convert&set pos
	Ivector2		bp;
	HUD().ClientToScreenScaled	(bp,iPos.x,iPos.y,uAlign);

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

