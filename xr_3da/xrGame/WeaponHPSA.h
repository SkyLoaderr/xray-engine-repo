#ifndef __XR_WEAPON_HPSA_H__
#define __XR_WEAPON_HPSA_H__
#pragma once

#include "WeaponCustomPistol.h"
 
class CWeaponHPSA: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
public:
					CWeaponHPSA			();
	virtual			~CWeaponHPSA		();
};

#endif //__XR_WEAPON_HPSA_H__
