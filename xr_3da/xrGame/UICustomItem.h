#ifndef __XR_UICUSTOMITEM_H__
#define __XR_UICUSTOMITEM_H__
#pragma once

enum EUIItemAlign{
	alNone	= 0x0000,
	alLeft	= 0x0001,
	alRight	= 0x0002,
	alTop	= 0x0004,
	alBottom= 0x0008,
	alCenter= 0x0010
};

class CUICustomItem
{
protected:
	enum {
		flValidRect=0x0001
	};

	Irect			iVisRect;
	u32				uFlags;
	u32				uAlign;
public:
					CUICustomItem	();
	virtual			~CUICustomItem	();
	IC void			SetRect			(int x1, int y1, int x2, int y2){iVisRect.set(x1,y1,x2,y2); uFlags|=flValidRect; }
	IC void			SetRect			(const Irect& r){iVisRect.set(r); uFlags|=flValidRect; }

	IC Irect		GetRect			(){return iVisRect;}

	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color, int x1, int y1, int x2, int y2);
	
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color);
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle);

	void			RenderTexPart	(FVF::TL*& Pointer, const Ivector2& pos, u32 color,  
														float x1, float y1,  
														float x2, float y2,
														float x3, float y3,
														float x4, float y4);

	IC void			SetAlign		(u32 align)					{uAlign=align;};
	IC u32			GetAlign		()							{return uAlign;}
};

#endif //__XR_UICUSTOMITEM_H__
