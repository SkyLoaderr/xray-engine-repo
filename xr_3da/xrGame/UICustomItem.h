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

enum EUIMirroring{
	tmNone,
	tmMirrorHorisontal,
	tmMirrorVertical,
	tmMirrorBoth
};

class CUICustomItem
{
protected:
	enum {
		flValidRect=0x0001,
		flValidOriginalRect=0x0002,
		flValidTextureRect=0x0004
	};

	//прямоугольник(в пикселях) 
	//часть участка, который выводится в данный момент
	Irect			iVisRect;
	//положение участка текстуры, который выводится на экран
	Irect			iOriginalRect;
	//размеры текстуры
	Irect			iTextureRect;


	float			fScale;

	u32				uFlags;
	u32				uAlign;
	EUIMirroring	eMirrorMode;

public:
					CUICustomItem	();
	virtual			~CUICustomItem	();
	IC void			SetRect			(int x1, int y1, int x2, int y2){iVisRect.set(x1,y1,x2,y2); uFlags|=flValidRect; }
	IC void			SetRect			(const Irect& r){iVisRect.set(r); uFlags|=flValidRect; }
	  void			SetOriginalRect	(int x, int y, int width, int height);

	IC Irect		GetRect					() {return iVisRect;}
	   Irect		GetOriginalRect			() const;
	   Irect		GetOriginalRectScaled	();


	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color, 
														int x1, int y1, 
														int x2, int y2);
	
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color);
	void			Render			(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle);

	void			RenderTexPart	(FVF::TL*& Pointer, const Ivector2& pos, u32 color,  
														float x1, float y1,  
														float x2, float y2,
														float x3, float y3,
														float x4, float y4);

	IC void			SetAlign		(u32 align)					{uAlign=align;};
	IC u32			GetAlign		()							{return uAlign;}

	IC void			SetMirrorMode	(EUIMirroring m)			{ eMirrorMode = m; }
	IC EUIMirroring GetMirrorMode	()							{ return eMirrorMode; }

	//для пропорционального масштабирования выводимой текстуры
	void			SetScale		(float new_scale);
	float			GetScale		();
};

#endif //__XR_UICUSTOMITEM_H__
