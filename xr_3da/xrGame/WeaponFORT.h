#ifndef __XR_WEAPON_FORT_H__
#define __XR_WEAPON_FORT_H__
#pragma once

#include "WeaponCustomPistol.h"

#define SND_RIC_COUNT 5
 
class CWeaponFORT: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
public:
					CWeaponFORT			();
	virtual			~CWeaponFORT		();
};

#endif //__XR_WEAPON_FORT_H__
