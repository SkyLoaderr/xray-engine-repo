#ifndef __XR_WEAPON_PISTOL_H__
#define __XR_WEAPON_PISTOL_H__
#pragma once

#include "WeaponMagazined.h"

#define SND_RIC_COUNT 5
 
class CWeaponCustomPistol: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
public:
					CWeaponCustomPistol	(LPCSTR name);
	virtual			~CWeaponCustomPistol();
protected:
	virtual void	FireEnd				();
	virtual void	switch2_Fire		();
	virtual void	state_Fire			(float dt);
	virtual void	state_MagEmpty		(float dt);
	virtual void	OnAnimationEnd		();

};

#endif //__XR_WEAPON_PISTOL_H__
