#pragma once
#include "weaponpistol.h"

class CWeaponRPG7 :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponRPG7(void);
	virtual ~CWeaponRPG7(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void OnStateSwitch(u32 S);

	static void	__stdcall GrenadeCallback(CBoneInstance*);

	bool m_hideGrenade;
};
