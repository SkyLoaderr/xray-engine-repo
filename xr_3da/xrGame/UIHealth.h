#ifndef __XR_UIHEALTH_H__
#define __XR_UIHEALTH_H__
#pragma once

#include "uistaticitem.h"

class CUIHealth
{
	int				health;
	int				armor;
	CUIStaticItem	back;
	CUIStaticItem	health_bar;
	CUIStaticItem	armor_bar;
	Frect			health_rect;
	Frect			armor_rect;
	Fcolor			s_color,m_color,e_color;
public:
					CUIHealth		();
	void			Init			();
	void			Render			();
	void			Out				(int _health, int _armor);
};

#endif //__XR_UIHEALTH_H__
