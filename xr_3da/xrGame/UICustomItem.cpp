#include "stdafx.h"
#include "uicustomitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICustomItem::CUICustomItem()
{    
	bInit		= false;
}
//--------------------------------------------------------------------

CUICustomItem::~CUICustomItem()
{
}
//--------------------------------------------------------------------
 
void CUICustomItem::Init(int tx_width, int tx_height)
{
	VERIFY			(!bInit);
	iTexSize.set	(tx_width,tx_height);
	iRect.lt.set	(0,0); iRect.rb.set(iTexSize);
	vHalfPixel.set	(0.5f/float(iTexSize.x),0.5f/float(iTexSize.y));
	bInit			= true;
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, DWORD color, int x1, int y1, int x2, int y2)
{
	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt;
	float sc		= Level().HUD()->GetScale();
	LTp.set			(pos.x+x1*sc,pos.y+y1*sc);
	RBp.set			(pos.x+x2*sc,pos.y+y2*sc);

	LTt.set			(float(x1)/float(iTexSize.x)+vHalfPixel.x,float(y1)/float(iTexSize.y)+vHalfPixel.y);
	RBt.set			(float(x2)/float(iTexSize.x)+vHalfPixel.x,float(y2)/float(iTexSize.y)+vHalfPixel.y);

	Pointer->set	(LTp.x,	RBp.y,	color, LTt.x, RBt.y); Pointer++;
	Pointer->set	(LTp.x,	LTp.y,	color, LTt.x, LTt.y); Pointer++;
	Pointer->set	(RBp.x,	RBp.y,	color, RBt.x, RBt.y); Pointer++;
	Pointer->set	(RBp.x,	LTp.y,	color, RBt.x, LTt.y); Pointer++;
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, DWORD color)
{
	Render(Pointer,pos,color,iRect.x1,iRect.y1,iRect.x2,iRect.y2);
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Ivector2& pos, DWORD color, float angle)
{
	float			_sin1,_cos1,_sin2,_cos2;
	angle			-=PI_DIV_4;
    _sincos			(angle,_sin1,_cos1); 
	angle			+=PI_DIV_2;
	_sincos			(angle,_sin2,_cos2);

	Ivector2 C,RS;
	iRect.getsize	(RS);
	float sc		= Level().HUD()->GetScale();
	float sz		= sc*((RS.x>RS.y)?RS.x:RS.y)*0.7071f;

	Fvector2 LTt,RBt;
	LTt.set			(float(iRect.x1)/float(iTexSize.x)+vHalfPixel.x,float(iRect.y1)/float(iTexSize.y)+vHalfPixel.y);
	RBt.set			(float(iRect.x2)/float(iTexSize.x)+vHalfPixel.x,float(iRect.y2)/float(iTexSize.y)+vHalfPixel.y);

	// Rotation
	iRect.getcenter	(C); 
	C.mul			(sc);
	C.add			(pos);

	Pointer->set	(C.x+_sin1*sz,	C.y+_cos1*sz,	color, LTt.x, RBt.y); Pointer++;
	Pointer->set	(C.x-_sin2*sz,	C.y-_cos2*sz,	color, LTt.x, LTt.y); Pointer++;
	Pointer->set	(C.x+_sin2*sz,	C.y+_cos2*sz,	color, RBt.x, RBt.y); Pointer++;
	Pointer->set	(C.x-_sin1*sz,	C.y-_cos1*sz,	color, RBt.x, LTt.y); Pointer++;
}
//--------------------------------------------------------------------
