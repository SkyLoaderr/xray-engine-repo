#ifndef __XR_UIWEAPON_H__
#define __XR_UIWEAPON_H__
#pragma once

#include "uistaticitem.h"

// refs
class CWeapon;

class CUIWeapon
{
	Fvector2		position;

	CUIStaticItem	back;
	CUIStaticItem	weapon;
	Shader*			cur_shader;
public:
					CUIWeapon		();
					~CUIWeapon		();
	void			Render			();
	void			Out				(CWeapon* wpn);
	void			Init			();
};

#endif //__XR_UIWEAPON_H__
