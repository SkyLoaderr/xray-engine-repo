#ifndef __XR_UILISTBOX_H__
#define __XR_UILISTBOX_H__
#pragma once

#include "uistaticitem.h"
#include "uiframerect.h"

class CUIListBox
{
protected:
	Irect			list_rect;
	CUIFrameRect	frame;
public:
					CUIListBox		();
	void			Init			(LPCSTR tex, int x1, int y1, int w, int h, DWORD align);
	virtual void	Render			();
	virtual void	OnFrame			();
};

#endif //__XR_UILISTBOX_H__
