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
	
	if (!(uFlags&flValidRect)){
		SetRect		(0,0,ts.x,ts.y);
	}
	if (!(uFlags&flValidOriginalRect)){
		iOriginalRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidOriginalRect;
	}
	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt;
	//координаты на экране в пикселях
	float scX		= UI()->GetScaleX();
	float scY		= UI()->GetScaleY();
	LTp.set			(pos.x+x1*scX,pos.y+y1*scY);
	RBp.set			(pos.x+x2*scX,pos.y+y2*scY);
	//текстурные координаты
	LTt.set			( fScaleX*float(iOriginalRect.x1)/float(ts.x)+hp.x,fScaleY*float(iOriginalRect.y1)/float(ts.y)+hp.y);
	RBt.set			( fScaleX*float(iOriginalRect.x2)/float(ts.x)+hp.x,fScaleY*float(iOriginalRect.y2)/float(ts.y)+hp.y);
	// Check mirror mode
	if (tmMirrorHorisontal == eMirrorMode || tmMirrorBoth == eMirrorMode)	std::swap	(LTt.x,RBt.x);
	if (tmMirrorVertical == eMirrorMode || tmMirrorBoth == eMirrorMode)		std::swap	(LTt.y,RBt.y);
	// clip poly
	sPoly2D			S; S.resize(4);
	S[0].set		(LTp.x,LTp.y,	LTt.x,LTt.y);	// LT
	S[1].set		(RBp.x,LTp.y,	RBt.x,LTt.y);	// RT
	S[2].set		(RBp.x,RBp.y,	RBt.x,RBt.y);	// RB
	S[3].set		(LTp.x,RBp.y,	LTt.x,RBt.y);	// LB
	
	sPoly2D D;
	sPoly2D* R		= UI()->ScreenFrustum().ClipPoly(S,D);
	if (R&&R->size()){
		for (u32 k=0; k<R->size(); k++,Pointer++)
			Pointer->set	((*R)[k].pt.x, (*R)[k].pt.y,	color, (*R)[k].uv.x, (*R)[k].uv.y); 
	}
}
//--------------------------------------------------------------------
void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color)
{
	Render(Pointer,pos,color,iVisRect.x1,iVisRect.y1,iVisRect.x2,iVisRect.y2);
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

	if (!(uFlags&flValidOriginalRect)){
		iOriginalRect.set(0,0,ts.x,ts.y);
		uFlags		|= flValidOriginalRect;
	}

	Fvector2		pivot,offset,SZ;
	SZ.set			(UI()->GetScaleX()*iVisRect.x2,UI()->GetScaleY()*iVisRect.y2);
	float cosA		= _cos(angle);
	float sinA		= _sin(angle);

	// Rotation
	if(!(uFlags&flValidHeadingPivot))	pivot.set(float(iVisRect.x2)/2.f,float(iVisRect.y2)/2.f);
	else								pivot.set(iHeadingPivot.x,iHeadingPivot.y);
	pivot.x			*= UI()->GetScaleX();
	pivot.y			*= UI()->GetScaleY();
	offset.set		(pos.x,pos.y);
	Fvector2		LTt,RBt;
	LTt.set			(fScaleX*float(iOriginalRect.x1)/float(ts.x)+hp.x, fScaleY*float(iOriginalRect.y1)/float(ts.y)+hp.y);
	RBt.set			(fScaleX*float(iOriginalRect.x2)/float(ts.x)+hp.x, fScaleY*float(iOriginalRect.y2)/float(ts.y)+hp.y);
	// Check mirror mode
	if (tmMirrorHorisontal == eMirrorMode || tmMirrorBoth == eMirrorMode)	std::swap	(LTt.x,RBt.x);
	if (tmMirrorVertical == eMirrorMode || tmMirrorBoth == eMirrorMode)		std::swap	(LTt.y,RBt.y);
	// clip poly
	sPoly2D			S; S.resize(4);
	// LT
	S[0].set		(0.f,0.f,LTt.x,LTt.y);
	S[0].rotate_pt	(pivot,cosA,sinA);
	S[0].pt.add		(offset);
	// RT
	S[1].set		(SZ.x,0.f,RBt.x,LTt.y);
	S[1].rotate_pt	(pivot,cosA,sinA);
	S[1].pt.add		(offset);
	// RB
	S[2].set		(SZ.x,SZ.y,RBt.x,RBt.y);
	S[2].rotate_pt	(pivot,cosA,sinA);
	S[2].pt.add		(offset);
	// LB
	S[3].set		(0.f,SZ.y,LTt.x,RBt.y);
	S[3].rotate_pt	(pivot,cosA,sinA);
	S[3].pt.add		(offset);
	
	sPoly2D D;
	sPoly2D* R		= UI()->ScreenFrustum().ClipPoly(S,D);
	if (R&&R->size())
		for (u32 k=0; k<R->size(); k++,Pointer++)
			Pointer->set	((*R)[k].pt.x, (*R)[k].pt.y,	color, (*R)[k].uv.x, (*R)[k].uv.y); 
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

void CUICustomItem::SetOriginalRect(int x, int y, int width, int height)
{
	iOriginalRect.set(x,y,x+width,y+height); 
	uFlags|=flValidOriginalRect; 
}
