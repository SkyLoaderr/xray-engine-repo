#ifndef __XR_WEAPON_FORT_H__
#define __XR_WEAPON_FORT_H__
#pragma once

#include "WeaponPistol.h"

#define SND_RIC_COUNT 5
 
class CWeaponFORT: public CWeaponPistol
{
private:
	typedef CWeaponPistol inherited;
protected:
public:
					CWeaponFORT			();
	virtual			~CWeaponFORT		();
};

#endif //__XR_WEAPON_FORT_H__
