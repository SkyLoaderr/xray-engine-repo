#ifndef __XR_UIFRAMERECT_H__
#define __XR_UIFRAMERECT_H__
#pragma once

#include "uistaticitem.h"

class CUIFrameRect
{
	enum{
		fmL=0,
		fmR,
		fmT,
		fmB,
		fmLT,
		fmRB,
		fmRT,
		fmLB,
		fmMax
	};
	CUIStaticItem	back;
	CUIStaticItem	frame[fmMax];

	enum {
		flValidSize	=0x0001
	};
protected:
	u32				uFlags;
	void			UpdateSize		();
public:
	Ivector2		iPos;
	Ivector2		iSize;
public:
					CUIFrameRect	();
	void			Init			(LPCSTR base_name, int x, int y, int w, int h, DWORD align);
	void			Render			();
	IC void			SetPos			(int left, int top)		{iPos.set(left,top);		uFlags &=~ flValidSize; }
	IC void			SetSize			(int width, int height)	{iSize.set(width,height);	uFlags &=~ flValidSize; }
};

#endif //__XR_UIFRAMERECT_H__
