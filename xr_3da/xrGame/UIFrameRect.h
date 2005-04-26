#ifndef __XR_UIFRAMERECT_H__
#define __XR_UIFRAMERECT_H__
#pragma once

#include "uistaticitem.h"

class CUIFrameRect: public CUICustomItem
{
	enum{
		fmBK=0,
		fmL,
		fmR,
		fmT,
		fmB,
		fmLT,
		fmRB,
		fmRT,
		fmLB,
		fmMax
	};
	CUIStaticItem	frame[fmMax];

	enum {
		flValidSize	= (1<<0),
		flSingleTex	= (1<<1),
	};
protected:
	Flags32			uFlags;
	void			UpdateSize		();
public:
	Ivector2		iPos;
	Ivector2		iSize;
public:
	friend class CUIFrameWindow;

					CUIFrameRect	();
	void			Init			(LPCSTR base_name, int x, int y, int w, int h, DWORD align);
	void			Render			();
	void			SetColor		(u32 cl);
	IC void			SetPos			(int left, int top)		{iPos.set(left,top);		uFlags.set(flValidSize,FALSE); }
	IC void			SetSize			(int width, int height)	{iSize.set(width,height);	uFlags.set(flValidSize,FALSE); }
};

#endif //__XR_UIFRAMERECT_H__
