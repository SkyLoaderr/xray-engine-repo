#ifndef __XR_UICUSTOMITEM_H__
#define __XR_UICUSTOMITEM_H__
#pragma once

enum EUIItemAlign{
	alNone	= 0x0000,
	alLeft	= 0x0001,
	alRight	= 0x0002,
	alTop	= 0x0004,
	alBottom= 0x0008
};

class CUICustomItem
{
	Fvector2		vHalfPixel;
	Fvector2		vTexSize;

	Frect			Rect;
	bool			bInit;
protected:
	void			Init			(float tx_width, float tx_height);
public:
					CUICustomItem	();
	virtual			~CUICustomItem	();
	IC void			SetRect			(float x1, float y1, float x2, float y2){Rect.set(x1,y1,x2,y2);}
	IC void			SetRect			(const Frect& r, float part_x, float part_y){Rect.lt.set(r.lt);Rect.rb.set(r.rb.x*part_x,r.rb.y*part_y);}
	IC void			SetRect			(const Frect& r){Rect.set(r);}
	const Frect&	GetRect			(){return Rect;}
	void			Render			(FVF::TL*& Pointer, const Fvector2& pos, DWORD color);
	void			Render			(FVF::TL*& Pointer, const Fvector2& pos, DWORD color, float angle);
};

#endif //__XR_UICUSTOMITEM_H__
