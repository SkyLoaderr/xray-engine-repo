#pragma once
#include "weaponpistol.h"

class CWeaponRPG7Grenade;

class CWeaponRPG7 :	public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;

	friend CWeaponRPG7Grenade;
public:
	CWeaponRPG7(void);
	virtual ~CWeaponRPG7(void);

	virtual BOOL net_Spawn		(LPVOID DC);
	virtual void OnStateSwitch	(u32 S);
	virtual void OnEvent		(NET_Packet& P, u16 type);
	virtual void ReloadMagazine	();
	virtual void Load			(LPCSTR section);
	virtual void switch2_Fire	();

	virtual void FireStart		();
	virtual void SwitchState	(u32 S);

	static void	__stdcall GrenadeCallback(CBoneInstance*);


protected:
	CWeaponRPG7Grenade *m_pGrenade;
	Fvector *m_pGrenadePoint;

	bool	m_hideGrenade;

	ref_str	m_sGrenadeBoneName;
	ref_str	m_sHudGrenadeBoneName;
};
