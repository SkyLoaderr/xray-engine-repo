#ifndef __XR_WEAPON_AK74_H__
#define __XR_WEAPON_AK74_H__
#pragma once

#include "WeaponAutoRifle.h"

class CWeaponAK74: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
					CWeaponAK74		();
	virtual			~CWeaponAK74	();
};

#endif //__XR_WEAPON_AK74_H__
