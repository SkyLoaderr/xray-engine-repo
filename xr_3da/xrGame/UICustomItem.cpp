#include "stdafx.h"
#include "uicustomitem.h"
#include "hudmanager.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICustomItem::CUICustomItem()
{    
	uAlign			= alNone;
	uFlags			= 0;
	iVisRect.lt.set	(0,0); iVisRect.rb.set(0,0);
	eMirrorMode		= tmNone;

	fScale = 1.f;
}
//--------------------------------------------------------------------

CUICustomItem::~CUICustomItem()
{
}
void CUICustomItem::SetScale(float new_scale)
{
	fScale = 1.f/new_scale;
}

float CUICustomItem::GetScale()
{
	return 1.f/fScale;
}
//--------------------------------------------------------------------
//для вывода части изображения
void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color, 
						   int x1, int y1, int x2, int y2)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	
	if (!(uFlags&flValidRect))
	{
		SetRect		(0,0,ts.x,ts.y);
	}

	if (!(uFlags&flValidOriginalRect))
	{
		iOriginalRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidOriginalRect;
	}
			
	if (!(uFlags&flValidTextureRect))
	{
		iTextureRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidTextureRect;
	}

	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt;
	
	//координаты на экране в пикселях
	float sc		= HUD().GetScale();
	LTp.set			(pos.x+x1*sc,pos.y+y1*sc);
	RBp.set			(pos.x+x2*sc,pos.y+y2*sc);

/*	LTp.set			(float(pos.x+x1),float(pos.y+y1));
	RBp.set			(pos.x + x1 + float(x2-x1)/fScale,
					 pos.y + y1 + float(y2-y1)/fScale);*/


	//текстурные координаты
	/*LTt.set			(fScale* float(x1)/float(ts.x)+hp.x,
					 fScale* float(y1)/float(ts.y)+hp.y);
	RBt.set			(fScale*float(x2)/float(ts.x)+hp.x,
					 fScale*float(y2)/float(ts.y)+hp.y);*/
	
	LTt.set			( float(iOriginalRect.x1+fScale*x1)/float(iTextureRect.width())+hp.x,
					  float(iOriginalRect.y1+fScale*y1)/float(iTextureRect.height())+hp.y);
	RBt.set			((float(iOriginalRect.x1+fScale*x1)+
					  fScale*float(x2-x1))/
					  float(iTextureRect.width())+hp.x,
					 (float(iOriginalRect.y1+fScale*y1)+
					  fScale*float(y2-y1))/
					  float(iTextureRect.height())+hp.y);
/*	RBt.set			((float(iOriginalRect.x1+x1)+
					  float(x2-x1))/
					  float(iTextureRect.width())+hp.x,
					 (float(iOriginalRect.y1+y1)+
					  float(y2-y1))/
					  float(iTextureRect.height())+hp.y);*/

	// Check mirror mode
	if (tmMirrorHorisontal == eMirrorMode || tmMirrorBoth == eMirrorMode)
	{
		float tmp	= LTt.x;
		LTt.x		= RBt.x;
		RBt.x		= tmp;
	}

	if (tmMirrorVertical == eMirrorMode || tmMirrorBoth == eMirrorMode)
	{
		float tmp	= LTt.y;
		LTt.y		= RBt.y;
		RBt.y		= tmp;
	}

	Pointer->set	(LTp.x,	RBp.y,	color, LTt.x, RBt.y); ++Pointer;
	Pointer->set	(LTp.x,	LTp.y,	color, LTt.x, LTt.y); ++Pointer;
	Pointer->set	(RBp.x,	RBp.y,	color, RBt.x, RBt.y); ++Pointer;
	Pointer->set	(RBp.x,	LTp.y,	color, RBt.x, LTt.y); ++Pointer;
}
//--------------------------------------------------------------------
void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color)
{
	Render(Pointer,pos,color,iVisRect.x1,iVisRect.y1,iVisRect.x2,iVisRect.y2);
	/*Render(Pointer,pos,color,iOriginalRect.x1,iOriginalRect.y1,
							 iOriginalRect.x2,iOriginalRect.y2);
							 */
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	float			_sin1,_cos1,_sin2,_cos2;
	angle			-=PI_DIV_4;
	_sin1=_sin(angle); _cos1=_cos(angle);
	angle			+=PI_DIV_2;
	_sin2=_sin(angle); _cos2=_cos(angle);

	Fvector2 C;
	Ivector2 RS;
	iVisRect.getsize(RS);
	float sc		= HUD().GetScale();
	float sz		= sc*((RS.x>RS.y)?RS.x:RS.y)*0.7071f;

	Fvector2 LTt,RBt;
	LTt.set			(fScale*float(iVisRect.x1)/float(ts.x)+hp.x,
					 fScale*float(iVisRect.y1)/float(ts.y)+hp.y);
	RBt.set			(fScale*float(iVisRect.x2)/float(ts.x)+hp.x,
					 fScale*float(iVisRect.y2)/float(ts.y)+hp.y);

	// Rotation
	iVisRect.getcenter(RS); 
	C.set			(RS.x*sc+pos.x,RS.y*sc+pos.y);

	Pointer->set	(C.x+_sin1*sz,	C.y+_cos1*sz,	color, LTt.x, RBt.y); ++Pointer;
	Pointer->set	(C.x-_sin2*sz,	C.y-_cos2*sz,	color, LTt.x, LTt.y); ++Pointer;
	Pointer->set	(C.x+_sin2*sz,	C.y+_cos2*sz,	color, RBt.x, RBt.y); ++Pointer;
	Pointer->set	(C.x-_sin1*sz,	C.y-_cos1*sz,	color, RBt.x, LTt.y); ++Pointer;
}
//--------------------------------------------------------------------
//render in a rect a specified part of texture
void CUICustomItem::RenderTexPart(FVF::TL*& Pointer, const Ivector2& pos, u32 color,  
													float x1, float y1,  
													float x2, float y2,
													float x3, float y3,
													float x4, float y4)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	
	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	if (!(uFlags&flValidOriginalRect))
	{
		iOriginalRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidOriginalRect;
	}
	if (!(uFlags&flValidTextureRect))
	{
		iTextureRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidTextureRect;
	}



	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt, LBt, RTt;
	float sc		= HUD().GetScale();
	LTp.set			(pos.x+iVisRect.x1*sc,pos.y+iVisRect.y1*sc);
	RBp.set			(pos.x+iVisRect.x2*sc,pos.y+iVisRect.y2*sc);

	LBt.set(x2 + hp.x, y2 + hp.y);
	LTt.set(x1 + hp.x, y1 + hp.y);
	RBt.set(x3 + hp.x, y3 + hp.y);
	RTt.set(x4 + hp.x, y4 + hp.y);

	Pointer->set	(LTp.x,	RBp.y,	color, LBt.x, LBt.y); ++Pointer;
	Pointer->set	(LTp.x,	LTp.y,	color, LTt.x, LTt.y); ++Pointer;
	Pointer->set	(RBp.x,	RBp.y,	color, RBt.x, RBt.y); ++Pointer;
	Pointer->set	(RBp.x,	LTp.y,	color, RTt.x, RTt.y); ++Pointer;
}

Irect CUICustomItem::GetOriginalRectScaled()
{
	Irect rect;
	rect.x1 = iOriginalRect.x1;
	rect.y1 = iOriginalRect.y1;
	rect.x2 = iFloor(0.5f + rect.x1 + float(iOriginalRect.x2-iOriginalRect.x1)*GetScale());
	rect.y2 = iFloor(0.5f + rect.y1 + float(iOriginalRect.y2-iOriginalRect.y1)*GetScale());

	return rect;
}

Irect CUICustomItem::GetOriginalRect() const
{
	Irect rect;
	rect.x1 = iOriginalRect.x1;
	rect.y1 = iOriginalRect.y1;
	rect.x2 = iFloor(0.5f +rect.x1 + float(iOriginalRect.x2-iOriginalRect.x1));
	rect.y2 = iFloor(0.5f +rect.y1 + float(iOriginalRect.y2-iOriginalRect.y1));

	return rect;
}
void CUICustomItem::SetOriginalRect(int x, int y, int width, int height)
{
	iOriginalRect.set(x,y,x+width,y+height); 
	uFlags|=flValidOriginalRect; 
}
