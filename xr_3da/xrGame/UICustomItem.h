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
	bool			bInit;
protected:
	Ivector2		iTexSize;
	Irect			iRect;
protected:
	void			Init			(int tx_width, int tx_height);
public:
					CUICustomItem	();
	virtual			~CUICustomItem	();
	IC void			SetRect			(int x1, int y1, int x2, int y2){iRect.set(x1,y1,x2,y2);}
//	IC void			SetRect			(const Irect& r, float part_x, float part_y){iRect.lt.set(r.lt);iRect.rb.set(iFloor(float(r.rb.x)*part_x),iFloor(float(r.rb.y)*part_y));}
	IC void			SetRect			(const Irect& r){iRect.set(r);}
	const Irect&	GetRect			(){return iRect;}
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, DWORD color, int x1, int y1, int x2, int y2);
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, DWORD color);
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, DWORD color, float angle);
};

#endif //__XR_UICUSTOMITEM_H__
