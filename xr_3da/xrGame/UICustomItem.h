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
		flValidRect				=0x0001,
		flValidOriginalRect		=0x0002,
		flValidHeadingPivot		=0x0004,
	};

	//пр€моугольник(в пиксел€х) 
	//геом. регион  на который нат€гикаетс€ текстура с текстурными координатами iOriginalRect
	Irect			iVisRect;

	//фрейм текстуры в пиксел€х отн. 0/0
	Irect			iOriginalRect;

	// точка, относительно которой примен€ем поворот
	Ivector2		iHeadingPivot;

	float			fScaleX;
	float			fScaleY;

	u32				uFlags;
	u32				uAlign;
	EUIMirroring	eMirrorMode;

public:
					CUICustomItem			();
	virtual			~CUICustomItem			();
	IC void			SetRect					(int x1, int y1, int x2, int y2){iVisRect.set(x1,y1,x2,y2); uFlags|=flValidRect; }
	IC void			SetRect					(const Irect& r){iVisRect.set(r); uFlags|=flValidRect; }
	  void			SetOriginalRect			(int x, int y, int width, int height);

	IC Irect		GetRect					() {return iVisRect;}
	   Irect		GetOriginalRect			() const;
	   Irect		GetOriginalRectScaled	();
	
	   void			SetHeadingPivot			(const Ivector2& p)		{iHeadingPivot=p; uFlags|=flValidHeadingPivot;}
	   Ivector2		GetHeadingPivot			()						{return iHeadingPivot;}
	   

	void			Render					(FVF::TL*& Pointer, const Ivector2& pos, u32 color, 
														int x1, int y1, 
														int x2, int y2);
	
	void			Render					(FVF::TL*& Pointer, const Ivector2& pos, u32 color);
	void			Render					(FVF::TL*& Pointer, const Ivector2& pos, u32 color, float angle);

	IC void			SetAlign				(u32 align)					{uAlign=align;};
	IC u32			GetAlign				()							{return uAlign;}

	IC void			SetMirrorMode			(EUIMirroring m)			{ eMirrorMode = m; }
	IC EUIMirroring GetMirrorMode			()							{ return eMirrorMode; }

	//дл€ пропорционального масштабировани€ выводимой текстуры
	void			SetScaleXY				(float x, float y);
	float			GetScaleX				();
	float			GetScaleY				();
};
