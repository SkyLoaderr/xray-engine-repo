#ifndef __XR_UILOGO_H__
#define __XR_UILOGO_H__
#pragma once

#include "uistaticitem.h"
#include "uidynamicitem.h"

class CUILogo
{
	CUIStaticItem	back;
public:
					CUILogo			();
					~CUILogo		();
	void			Render			();
	void			Init			(LPCSTR tex_name);
};

#endif //__XR_UILOGO_H__
