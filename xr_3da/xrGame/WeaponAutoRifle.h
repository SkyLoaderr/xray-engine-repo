#ifndef __XR_WEAPON_AUTORIFLE_H__
#define __XR_WEAPON_AUTORIFLE_H__
#pragma once

#include "WeaponMagazined.h"

class CWeaponAutoRifleA: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
					CWeaponAutoRifleA	(LPCSTR name);
	virtual			~CWeaponAutoRifleA	();
};

#endif //__XR_WEAPON_AUTORIFLE_H__
