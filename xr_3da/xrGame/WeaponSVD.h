#pragma once
#include "weaponcustompistol.h"

class CWeaponSVD :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
protected:
	virtual void switch2_Fire	();
	virtual void OnAnimationEnd ();
public:
	CWeaponSVD(void);
	virtual ~CWeaponSVD(void);
};
