#ifndef __XR_WEAPON_PISTOL_H__
#define __XR_WEAPON_PISTOL_H__
#pragma once

#include "WeaponMagazined.h"

#define SND_RIC_COUNT 5
 
class CWeaponCustomPistol: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
protected:
	virtual void	switch2_Fire		(BOOL bHUDView);
	virtual void	switch2_Empty		(BOOL bHUDView);
	virtual void	OnEmptyClick		(BOOL bHUDView);
	virtual void	state_Fire			(BOOL bHUD, float dt);
	virtual void	state_MagEmpty		(BOOL bHUD, float dt);
public:
					CWeaponCustomPistol	(LPCSTR name);
	virtual			~CWeaponCustomPistol();
};

#endif //__XR_WEAPON_PISTOL_H__
