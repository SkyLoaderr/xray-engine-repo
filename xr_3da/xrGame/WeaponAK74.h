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

	void		Load	(LPCSTR section);
	BOOL		net_Spawn	(LPVOID DC); 
};

#endif //__XR_WEAPON_AK74_H__
