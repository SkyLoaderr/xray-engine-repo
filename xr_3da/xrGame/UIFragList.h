#ifndef __XR_UIFRAGLIST_H__
#define __XR_UIFRAGLIST_H__
#pragma once

#include "uistaticitem.h"
#include "uiframerect.h"

class CUIFragList
{
	Frect			list_rect;
	CUIFrameRect	frame;
public:
					CUIFragList		();
	void			Init			();
	void			Render			();
	void			OnFrame			();
};

#endif //__XR_UIFRAGLIST_H__
