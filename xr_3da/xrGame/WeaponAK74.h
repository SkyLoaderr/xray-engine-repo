#ifndef __XR_WEAPON_AK74_H__
#define __XR_WEAPON_AK74_H__
#pragma once

#include "WeaponMagazinedWGrenade.h"

class CWeaponAK74: public CWeaponMagazinedWGrenade
{
private:
	typedef CWeaponMagazinedWGrenade inherited;
public:
					CWeaponAK74		(LPCSTR name="AK74",ESoundTypes eSoundType=SOUND_TYPE_WEAPON_SUBMACHINEGUN);
	virtual			~CWeaponAK74	();
};

#include <luabind/function.hpp>
#include <luabind/class.hpp>

class CWeaponAK74Lua :public CWeaponAK74, luabind::wrap_base
{
public:
	CWeaponAK74Lua		(LPCSTR name="AK74",ESoundTypes eSoundType=SOUND_TYPE_WEAPON_SUBMACHINEGUN)
		:CWeaponAK74(name,eSoundType){};

};
#endif //__XR_WEAPON_AK74_H__
