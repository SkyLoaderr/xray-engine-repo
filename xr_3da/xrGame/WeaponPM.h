#ifndef __XR_WEAPON_PM_H__
#define __XR_WEAPON_PM_H__
#pragma once

#include "WeaponCustomPistol.h"

class CWeaponPM: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
public:
					CWeaponPM			();
	virtual			~CWeaponPM		();
};

#endif //__XR_WEAPON_PM_H__
