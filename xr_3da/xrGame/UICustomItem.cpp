#include "stdafx.h"
#include "uicustomitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICustomItem::CUICustomItem()
{    
	uAlign				= alNone;
	uFlags				= 0;
	iVisRect.set		(0,0,0,0);
	iOriginalRect.set	(0,0,0,0);
	iTextureRect.set	(0,0,0,0);
	eMirrorMode			= tmNone;
	fScaleX				= 1.f;
	fScaleY				= 1.f;
	iHeadingPivot.set	(0,0); 
}
//--------------------------------------------------------------------

CUICustomItem::~CUICustomItem()
{
}
void CUICustomItem::SetScaleXY(float x, float y)
{
	fScaleX = 1.f/x;
	fScaleY = 1.f/y;
}

float CUICustomItem::GetScaleX()
{
	return 1.f/fScaleX;
}

float CUICustomItem::GetScaleY()
{
	return 1.f/fScaleY;
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
	float scX		= UI()->GetScaleX();
	float scY		= UI()->GetScaleY();
	LTp.set			(pos.x+x1*scX,pos.y+y1*scY);
	RBp.set			(pos.x+x2*scX,pos.y+y2*scY);



	//текстурные координаты
	
	LTt.set			( float(fScaleX*iOriginalRect.x1)/float(iTextureRect.width())+hp.x,
					  float(fScaleY*iOriginalRect.y1)/float(iTextureRect.height())+hp.y);
	RBt.set			( float(fScaleX*iOriginalRect.x2)/float(iTextureRect.width())+hp.x,
					  float(fScaleY*iOriginalRect.y2)/float(iTextureRect.height())+hp.y);
/*	RBt.set			((float(iOriginalRect.x1+fScaleX*x1)+
					  fScaleX*float(x2-x1))/
					  float(iTextureRect.width())+hp.x,
					 (float(iOriginalRect.y1+fScaleY*y1)+
					  fScaleY*float(y2-y1))/
					  float(iTextureRect.height())+hp.y);
*/
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
}
//--------------------------------------------------------------------

void rotation(int x, int y, const float angle, int& x_, int& y_)
{
	x_= iFloor(float(x)*_cos(angle)+float(y)*_sin(angle));
	y_= iFloor(float(y)*_cos(angle)-float(x)*_sin(angle));
}
/*
void rotation(const float x, const float y, const float angle, float& x_, float& y_)
{
	x_= x*cos(angle)+y*sin(angle);
	y_= y*cos(angle)-x*sin(angle);
}
void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	angle = -angle;
	float scX		= UI()->GetScaleX();
	float scY		= UI()->GetScaleY();

	Fvector2 LTp,RBp;
	LTp.set			(pos.x+iOriginalRect.x1*scX, pos.y+iOriginalRect.y1*scY);
	RBp.set			(pos.x+iOriginalRect.x2*scX, pos.y+iOriginalRect.y2*scY);

	Fvector2 LTt,RTt,LBt,RBt;

	LTt.x = fScaleX*float(iOriginalRect.x1)/float(ts.x)+hp.x;
	LTt.y = fScaleY*float(iOriginalRect.y1)/float(ts.y)+hp.y;
	RBt.x = fScaleX*float(iOriginalRect.x2)/float(ts.x)+hp.x;
	RBt.y = fScaleY*float(iOriginalRect.y2)/float(ts.y)+hp.y;

	Fvector2 vis_center;
	vis_center.sub(RBt,LTt);
	vis_center.mul(0.5f);

	LTt.sub(vis_center);
	rotation(LTt.x,LTt.y,angle,LTt.x,LTt.y);
	LTt.add(vis_center);


	RBt.sub(vis_center);
	rotation(RBt.x,RBt.y,angle,RBt.x,RBt.y);
	RBt.add(vis_center);

	RTt.x = fScaleX*float(iOriginalRect.x2)/float(ts.x)+hp.x;
	RTt.y = fScaleY*float(iOriginalRect.y1)/float(ts.y)+hp.y;
	RTt.sub(vis_center);
	rotation(RTt.x,RTt.y,angle,RTt.x,RTt.y);
	RTt.add(vis_center);

	LBt.x = fScaleX*float(iOriginalRect.x1)/float(ts.x)+hp.x;
	LBt.y = fScaleY*float(iOriginalRect.y2)/float(ts.y)+hp.y;
	LBt.sub(vis_center);
	rotation(LBt.x,LBt.y,angle,LBt.x,LBt.y);
	LBt.add(vis_center);

	Pointer->set	(LTp.x,	RBp.y,	color, LBt.x, LBt.y); ++Pointer;
	Pointer->set	(LTp.x,	LTp.y,	color, LTt.x, LTt.y); ++Pointer;
	Pointer->set	(RBp.x,	RBp.y,	color, RBt.x, RBt.y); ++Pointer;
	Pointer->set	(RBp.x,	LTp.y,	color, RTt.x, RTt.y); ++Pointer;
}*/



void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	if (!(uFlags&flValidOriginalRect)){
		iOriginalRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidOriginalRect;
	}

//	float			_sin1,_cos1,_sin2,_cos2;
//	angle			-=PI_DIV_4;
//	_sin1=_sin(angle); 
//	_cos1=_cos(angle);
//	angle			+=PI_DIV_2;
//	_sin2=_sin(angle); 
//	_cos2=_cos(angle);

	Fvector2 C;
	Ivector2 RS;
	iVisRect.getsize(RS);
	float scX		= UI()->GetScaleX();
	float scY		= UI()->GetScaleY();
	float szX		= scX*(RS.x)*0.7071f;
	float szY		= scY*(RS.y)*0.7071f;

	Fvector2 LTt,RBt;
	LTt.set			(fScaleX*float(iOriginalRect.x1)/float(ts.x)+hp.x,
					 fScaleY*float(iOriginalRect.y1)/float(ts.y)+hp.y);
	RBt.set			(fScaleX*float(iOriginalRect.x2)/float(ts.x)+hp.x,
					 fScaleY*float(iOriginalRect.y2)/float(ts.y)+hp.y);
/*

	// Rotation
	if( !(uFlags&flValidHeadingPivot) ){
		iVisRect.getcenter(RS);
	}else
		RS = iHeadingPivot;

	C.set	(RS.x*scX+pos.x,RS.y*scY+pos.y);


	Pointer->set	(C.x+_sin1*szX,	C.y+_cos1*szY,	color, LTt.x, RBt.y); ++Pointer;
	Pointer->set	(C.x-_sin2*szX,	C.y-_cos2*szY,	color, LTt.x, LTt.y); ++Pointer;
	Pointer->set	(C.x+_sin2*szX,	C.y+_cos2*szY,	color, RBt.x, RBt.y); ++Pointer;
	Pointer->set	(C.x-_sin1*szX,	C.y-_cos1*szY,	color, RBt.x, LTt.y); ++Pointer;
*/
	// Rotation
	if( !(uFlags&flValidHeadingPivot) ){
		iVisRect.getcenter(RS);
	}else
		RS = iHeadingPivot;

	Ivector2 LTp,RTp,LBp,RBp;

	LTp.x = iVisRect.x1;
	LTp.y = iVisRect.y1;
	LTp.sub(RS);
	rotation(LTp.x,LTp.y,angle,LTp.x,LTp.y);
	LTp.add(RS);

	RBp.x = iVisRect.x2;
	RBp.y = iVisRect.y2;
	RBp.sub(RS);
	rotation(RBp.x,RBp.y,angle,RBp.x,RBp.y);
	RBp.add(RS);

	RTp.x = iVisRect.x2;
	RTp.y = iVisRect.y1;
	RTp.sub(RS);
	rotation(RTp.x,RTp.y,angle,RTp.x,RTp.y);
	RTp.add(RS);

	LBp.x = iVisRect.x1;
	LBp.y = iVisRect.y2;
	LBp.sub(RS);
	rotation(LBp.x,LBp.y,angle,LBp.x,LBp.y);
	LBp.add(RS);

	Pointer->set	(LBp.x+pos.x, LBp.y+pos.y,	color, LTt.x, RBt.y); ++Pointer;
	Pointer->set	(LTp.x+pos.x, LTp.y+pos.y,	color, LTt.x, LTt.y); ++Pointer;
	Pointer->set	(RBp.x+pos.x, RBp.y+pos.y,	color, RBt.x, RBt.y); ++Pointer;
	Pointer->set	(RTp.x+pos.x, RTp.y+pos.y,	color, RBt.x, LTt.y); ++Pointer;
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
	float scX		= UI()->GetScaleX();
	float scY		= UI()->GetScaleY();
	LTp.set			(pos.x+iVisRect.x1*scX,pos.y+iVisRect.y1*scY);
	RBp.set			(pos.x+iVisRect.x2*scX,pos.y+iVisRect.y2*scY);

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
	Irect rect = iOriginalRect;

	rect.x2		= rect.x1 + iFloor(rect.width()	 * GetScaleX() );
	rect.y2		= rect.y1 + iFloor(rect.height() * GetScaleY() );

	return rect;
}

Irect CUICustomItem::GetOriginalRect() const
{
	return iOriginalRect;
}
/*
Irect CUICustomItem::GetOriginalRectScaled()
{
	Irect rect;
	rect.x1 = iOriginalRect.x1;
	rect.y1 = iOriginalRect.y1;
	rect.x2 = iFloor(0.5f + rect.x1 + float(iOriginalRect.x2-iOriginalRect.x1)*GetScaleX());
	rect.y2 = iFloor(0.5f + rect.y1 + float(iOriginalRect.y2-iOriginalRect.y1)*GetScaleY());

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
}*/


void CUICustomItem::SetOriginalRect(int x, int y, int width, int height)
{
	iOriginalRect.set(x,y,x+width,y+height); 
	uFlags|=flValidOriginalRect; 
}
