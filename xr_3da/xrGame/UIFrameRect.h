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
public:
	Irect			rect;
public:
					CUIFrameRect	();
	void			Init			(LPCSTR base_name, int x, int y, int w, int h, DWORD align);
	void			Render			();
};

#endif //__XR_UIFRAMERECT_H__
