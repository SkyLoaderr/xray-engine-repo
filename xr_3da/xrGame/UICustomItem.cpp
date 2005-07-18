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
	iHeadingPivot.set	(0,0); 
}
//--------------------------------------------------------------------

CUICustomItem::~CUICustomItem()
{
}
//--------------------------------------------------------------------
//��� ������ ����� �����������
void CUICustomItem::Render(FVF::TL*& Pointer, const Fvector2& pos, u32 color, 
						   float x1, float y1, float x2, float y2)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Fvector2		ts;
	ts.set			(float(T->get_Width()),float(T->get_Height()));
	
	if (!(uFlags&flValidRect)){
		SetRect		(0,0,ts.x,ts.y);
	}
	if (!(uFlags&flValidOriginalRect)){
		iOriginalRect.set(0,0,ts.x,ts.y);
		uFlags |= flValidOriginalRect;
	}

//.	VERIFY			(uFlags&flValidRect);
//.	VERIFY			(uFlags&flValidOriginalRect);

	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt;
	//���������� �� ������ � ��������
	float scX		= UI()->GetScaleX();
	float scY		= UI()->GetScaleY();
	LTp.set			(pos.x+x1*scX, pos.y+y1*scY );
	RBp.set			(pos.x+x2*scX, pos.y+y2*scY );
//.	LTp.set			(iFloor(pos.x+float(x1)*scX), iFloor(pos.y+float(y1)*scY) );
//.	RBp.set			(iFloor(pos.x+float(x2)*scX), iFloor(pos.y+float(y2)*scY) );
	//���������� ����������
	LTt.set			( iOriginalRect.x1/ts.x, iOriginalRect.y1/ts.y);
	RBt.set			( iOriginalRect.x2/ts.x, iOriginalRect.y2/ts.y);
//.	LTt.set			( float(iOriginalRect.x1)/float(ts.x),float(iOriginalRect.y1)/float(ts.y));
//.	RBt.set			( float(iOriginalRect.x2)/float(ts.x),float(iOriginalRect.y2)/float(ts.y));

	// Check mirror mode
	if (tmMirrorHorisontal == eMirrorMode || tmMirrorBoth == eMirrorMode)	std::swap	(LTt.x,RBt.x);
	if (tmMirrorVertical == eMirrorMode || tmMirrorBoth == eMirrorMode)		std::swap	(LTt.y,RBt.y);
	// clip poly
	sPoly2D			S; S.resize(4);
	S[0].set		(LTp.x-0.5f,LTp.y-0.5f,	LTt.x,LTt.y);	// LT
	S[1].set		(RBp.x-0.5f,LTp.y-0.5f,	RBt.x,LTt.y);	// RT
	S[2].set		(RBp.x-0.5f,RBp.y-0.5f,	RBt.x,RBt.y);	// RB
	S[3].set		(LTp.x-0.5f,RBp.y-0.5f,	LTt.x,RBt.y);	// LB
	
	sPoly2D D;
	sPoly2D* R		= UI()->ScreenFrustum().ClipPoly(S,D);
	if (R&&R->size()){
		for (u32 k=0; k<R->size()-2; k++){
			Pointer->set	((*R)[0+0].pt.x, (*R)[0+0].pt.y,	color, (*R)[0+0].uv.x, (*R)[0+0].uv.y); Pointer++;
			Pointer->set	((*R)[k+1].pt.x, (*R)[k+1].pt.y,	color, (*R)[k+1].uv.x, (*R)[k+1].uv.y); Pointer++;
			Pointer->set	((*R)[k+2].pt.x, (*R)[k+2].pt.y,	color, (*R)[k+2].uv.x, (*R)[k+2].uv.y); Pointer++;
		}
	}
}
//--------------------------------------------------------------------
void CUICustomItem::Render(FVF::TL*& Pointer, const Fvector2& pos, u32 color)
{
	Render(Pointer,pos,color,iVisRect.x1,iVisRect.y1,iVisRect.x2,iVisRect.y2);
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Fvector2& pos, u32 color, float angle)
{
	CTexture* T		= RCache.get_ActiveTexture(0);
	Fvector2		ts;
	Fvector2		hp;
	ts.set			(float(T->get_Width()),float(T->get_Height()));
	hp.set			(0.5f/ts.x,0.5f/ts.y);

	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	if (!(uFlags&flValidOriginalRect)){
		iOriginalRect.set(0,0,-ts.x,ts.y);
		uFlags		|= flValidOriginalRect;
	}
/*	
	VERIFY			(uFlags&flValidRect);
	VERIFY			(uFlags&flValidOriginalRect);
*/

	Fvector2		pivot,offset,SZ;
	SZ.set			(UI()->GetScaleX()*iVisRect.x2,UI()->GetScaleY()*iVisRect.y2);
	float cosA		= _cos(angle);
	float sinA		= _sin(angle);

	// Rotation
	if(!(uFlags&flValidHeadingPivot))	pivot.set(iVisRect.x2/2.f, iVisRect.y2/2.f);
	else								pivot.set(iHeadingPivot.x, iHeadingPivot.y);
	pivot.x			*= UI()->GetScaleX();
	pivot.y			*= UI()->GetScaleY();
	offset.set		(pos.x,pos.y);
	Fvector2		LTt,RBt;
	LTt.set			(iOriginalRect.x1/ts.x+hp.x, iOriginalRect.y1/ts.y+hp.y);
	RBt.set			(iOriginalRect.x2/ts.x+hp.x, iOriginalRect.y2/ts.y+hp.y);
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

Frect CUICustomItem::GetOriginalRectScaled()
{
	Frect rect = iOriginalRect;

	rect.x2		= rect.x1 + rect.width();
	rect.y2		= rect.y1 + rect.height();

	return rect;
}

Frect CUICustomItem::GetOriginalRect() const
{
	return iOriginalRect;
}

void CUICustomItem::SetOriginalRect(float x, float y, float width, float height)
{
	iOriginalRect.set(x,y,x+width,y+height); 
	uFlags|=flValidOriginalRect; 
}
