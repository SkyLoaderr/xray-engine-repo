#ifndef __XR_WEAPON_HPSA_H__
#define __XR_WEAPON_HPSA_H__
#pragma once

#include "WeaponAutoRifle.h"

class CWeaponHPSA: public CWeaponAutoRifle
{
private:
	typedef CWeaponAutoRifle inherited;
public:
					CWeaponHPSA		();
	virtual			~CWeaponHPSA	();
	virtual	void	OnShot			(BOOL bHUDView);
	virtual void	switch2_Fire	(BOOL bHUDView);
};

#endif //__XR_WEAPON_AK74_H__
