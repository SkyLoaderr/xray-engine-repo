#include "stdafx.h"
#include "uicustomitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICustomItem::CUICustomItem()
{    
	uAlign			= alNone;
	uFlags			= 0;
	iVisRect.lt.set	(0,0); iVisRect.rb.set(0,0);
}
//--------------------------------------------------------------------

CUICustomItem::~CUICustomItem()
{
}
//--------------------------------------------------------------------
 
void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color, int x1, int y1, int x2, int y2)
{
	CTexture* T		= Device.Shader.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt;
	float sc		= Level().HUD()->GetScale();
	LTp.set			(pos.x+x1*sc,pos.y+y1*sc);
	RBp.set			(pos.x+x2*sc,pos.y+y2*sc);

	LTt.set			(float(x1)/float(ts.x)+hp.x,float(y1)/float(ts.y)+hp.y);
	RBt.set			(float(x2)/float(ts.x)+hp.x,float(y2)/float(ts.y)+hp.y);

	Pointer->set	(LTp.x,	RBp.y,	color, LTt.x, RBt.y); Pointer++;
	Pointer->set	(LTp.x,	LTp.y,	color, LTt.x, LTt.y); Pointer++;
	Pointer->set	(RBp.x,	RBp.y,	color, RBt.x, RBt.y); Pointer++;
	Pointer->set	(RBp.x,	LTp.y,	color, RBt.x, LTt.y); Pointer++;
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color)
{
	Render(Pointer,pos,color,iVisRect.x1,iVisRect.y1,iVisRect.x2,iVisRect.y2);
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle)
{
	CTexture* T		= Device.Shader.get_ActiveTexture(0);
	Ivector2		ts;
	Fvector2		hp;
	ts.set			((int)T->get_Width(),(int)T->get_Height());
	hp.set			(0.5f/float(ts.x),0.5f/float(ts.y));
	if (!(uFlags&flValidRect))	SetRect		(0,0,ts.x,ts.y);

	float			_sin1,_cos1,_sin2,_cos2;
	angle			-=PI_DIV_4;
    _sincos			(angle,_sin1,_cos1); 
	angle			+=PI_DIV_2;
	_sincos			(angle,_sin2,_cos2);

	Fvector2 C;
	Ivector2 RS;
	iVisRect.getsize(RS);
	float sc		= Level().HUD()->GetScale();
	float sz		= sc*((RS.x>RS.y)?RS.x:RS.y)*0.7071f;

	Fvector2 LTt,RBt;
	LTt.set			(float(iVisRect.x1)/float(ts.x)+hp.x,float(iVisRect.y1)/float(ts.y)+hp.y);
	RBt.set			(float(iVisRect.x2)/float(ts.x)+hp.x,float(iVisRect.y2)/float(ts.y)+hp.y);

	// Rotation
	iVisRect.getcenter(RS); 
	C.set			(RS.x*sc+pos.x,RS.y*sc+pos.y);

	Pointer->set	(C.x+_sin1*sz,	C.y+_cos1*sz,	color, LTt.x, RBt.y); Pointer++;
	Pointer->set	(C.x-_sin2*sz,	C.y-_cos2*sz,	color, LTt.x, LTt.y); Pointer++;
	Pointer->set	(C.x+_sin2*sz,	C.y+_cos2*sz,	color, RBt.x, RBt.y); Pointer++;
	Pointer->set	(C.x-_sin1*sz,	C.y-_cos1*sz,	color, RBt.x, LTt.y); Pointer++;
}
//--------------------------------------------------------------------
