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
	enum {
		flValidRect=0x0001
	};
protected:
	Irect			iRect;
	u32				uFlags;
public:
					CUICustomItem	();
	virtual			~CUICustomItem	();
	IC void			SetRect			(int x1, int y1, int x2, int y2){iRect.set(x1,y1,x2,y2); uFlags|=flValidRect; }
	IC void			SetRect			(const Irect& r){iRect.set(r); uFlags|=flValidRect; }
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color, int x1, int y1, int x2, int y2);
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color);
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle);
};

#endif //__XR_UICUSTOMITEM_H__
