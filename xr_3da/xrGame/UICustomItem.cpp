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
 
void CUICustomItem::Init(float tx_width, float tx_height){
	VERIFY(!bInit);
	vTexSize.set(tx_width,tx_height);
	Rect.lt.set	(0.f,0.f); Rect.rb.set(vTexSize);
	vHalfPixel.set(0.5f/vTexSize.x,0.5f/vTexSize.y);
	bInit		= true;
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Fvector2& pos, DWORD color)
{
	Fvector2 LTp,RBp;
	Fvector2 LTt,RBt;
	float sc		= Level().HUD()->GetScale();
	LTp.direct		(pos,Rect.lt,sc);
	RBp.direct		(pos,Rect.rb,sc);

	LTt.set			(Rect.x1/vTexSize.x+vHalfPixel.x,Rect.y1/vTexSize.y+vHalfPixel.y);
	RBt.set			(Rect.x2/vTexSize.x+vHalfPixel.x,Rect.y2/vTexSize.y+vHalfPixel.y);
	
	Pointer->set	(LTp.x,	RBp.y,	color, LTt.x, RBt.y); Pointer++;
	Pointer->set	(LTp.x,	LTp.y,	color, LTt.x, LTt.y); Pointer++;
	Pointer->set	(RBp.x,	RBp.y,	color, RBt.x, RBt.y); Pointer++;
	Pointer->set	(RBp.x,	LTp.y,	color, RBt.x, LTt.y); Pointer++;
}
//--------------------------------------------------------------------

void CUICustomItem::Render(FVF::TL*& Pointer, const Fvector2& pos, DWORD color, float angle){
	float			_sin1,_cos1,_sin2,_cos2;
	angle			-=PI_DIV_4;
    _sincos			(angle,_sin1,_cos1); angle+=PI_DIV_2;
	_sincos			(angle,_sin2,_cos2);

	Fvector2 C,RS;
	Rect.getsize	(RS);
	float sc		= Level().HUD()->GetScale();
	float sz		= sc*((RS.x>RS.y)?RS.x:RS.y)*0.7071f;

	Fvector2 LTt,RBt;
	LTt.set			(Rect.x1/vTexSize.x+vHalfPixel.x,Rect.y1/vTexSize.y+vHalfPixel.y);
	RBt.set			(Rect.x2/vTexSize.x+vHalfPixel.x,Rect.y2/vTexSize.y+vHalfPixel.y);

	// Rotation
	Rect.getcenter	(C); 
	C.mul			(sc);
	C.add			(pos);

	Pointer->set	(C.x+_sin1*sz,	C.y+_cos1*sz,	color, LTt.x, RBt.y); Pointer++;
	Pointer->set	(C.x-_sin2*sz,	C.y-_cos2*sz,	color, LTt.x, LTt.y); Pointer++;
	Pointer->set	(C.x+_sin2*sz,	C.y+_cos2*sz,	color, RBt.x, RBt.y); Pointer++;
	Pointer->set	(C.x-_sin1*sz,	C.y-_cos1*sz,	color, RBt.x, LTt.y); Pointer++;
}
//--------------------------------------------------------------------
