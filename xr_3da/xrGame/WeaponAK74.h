#ifndef __XR_WEAPON_AK74_H__
#define __XR_WEAPON_AK74_H__
#pragma once

#include "WeaponMagazinedWGrenade.h"

class CWeaponAK74: public CWeaponMagazinedWGrenade
{
private:
	typedef CWeaponMagazinedWGrenade inherited;
public:
					CWeaponAK74		();
	virtual			~CWeaponAK74	();
};

#endif //__XR_WEAPON_AK74_H__
