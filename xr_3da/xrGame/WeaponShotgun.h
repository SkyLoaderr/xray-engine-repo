#pragma once
#include "weaponcustompistol.h"

class CWeaponShotgun :
	public CWeaponCustomPistol
{
public:
	CWeaponShotgun(void);
	~CWeaponShotgun(void);

	virtual void	OnAnimationEnd	();

	bool m_reload;
};
