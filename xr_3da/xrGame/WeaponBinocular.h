#ifndef __XR_WEAPON_BINOCULAR_H__
#define __XR_WEAPON_BINOCULAR_H__
#pragma once

#include "WeaponCustomPistol.h"
 
class CWeaponBinocular: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
public:
					CWeaponBinocular	();
	virtual			~CWeaponBinocular	();
};

#endif //__XR_WEAPON_BINOCULAR_H__
