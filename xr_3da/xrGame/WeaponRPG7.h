#pragma once
#include "weaponpistol.h"

class CWeaponRPG7Grenade :
	public CGameObject
{
	typedef CGameObject inherited;
public:
	CWeaponRPG7Grenade(void);
	virtual ~CWeaponRPG7Grenade(void);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	Fvector m_pos, m_vel;
};

class CWeaponRPG7 :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponRPG7(void);
	virtual ~CWeaponRPG7(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void OnStateSwitch(u32 S);
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void ReloadMagazine();
	//virtual void OnShot();
	virtual void	switch2_Fire	();

	static void	__stdcall GrenadeCallback(CBoneInstance*);

	bool m_hideGrenade;
	CWeaponRPG7Grenade *m_pGrenade;
};
