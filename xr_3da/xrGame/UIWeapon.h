#ifndef __XR_UIWEAPON_H__
#define __XR_UIWEAPON_H__
#pragma once

#include "uistaticitem.h"

class CUIWeaponItem: public CUIStaticItem{
public:
					CUIWeaponItem	(LPCSTR tex_name);
	virtual			~CUIWeaponItem	();
};

DEFINE_MAP(LPCSTR,CUIWeaponItem*,WItmMap,WItmIt);

class CUIWeapon
{
	Fvector2		position;

	WItmMap			weapons;
	CUIStaticItem	back;

	CUIWeaponItem*	current;
public:
					CUIWeapon		();
					~CUIWeapon		();
	void			Render			();
	void			Out				(LPCSTR wpn_name, int ammo1, int ammo1_total, int ammo2, int ammo2_total);
	void			Init			();
};

#endif //__XR_UIWEAPON_H__
