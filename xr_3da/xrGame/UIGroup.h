#ifndef __XR_UIGROUPS_H__
#define __XR_UIGROUPS_H__
#pragma once

#include "uistaticitem.h"

class CGroup;
class CSquad;

class CUIGroup{
	CUIStaticItem	back;
	CUIStaticItem	back_sel;
	CUIStaticItem	list_top;
	CUIStaticItem	list_item;
	CUIStaticItem	list_bottom;
	CUIStaticItem	list_item_health;
	CUIStaticItem	list_item_ammo;
public:
					CUIGroup		();
	void			Render			(CGroup& G, int offs, int grp_index, bool bSelected);
};

class CUISquad
{
	CUIGroup		group;
public:
					CUISquad		();
					~CUISquad		();
	void			Init			();
	void			Render			(CSquad& S, bool* bSel, bool bActive);
};

#endif //__XR_UIGROUPS_H__
