#pragma once
#include "weaponpistol.h"
#include "rocketlauncher.h"


class CWeaponRPG7 :	public CWeaponCustomPistol,
					public CRocketLauncher
{
private:
	typedef CWeaponCustomPistol inherited;
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

	virtual void UpdateGrenadeVisibility(bool visibility);
protected:
	Fvector *m_pGrenadePoint;

	ref_str	m_sGrenadeBoneName;
	ref_str	m_sHudGrenadeBoneName;

	ref_str m_sRocketSection;
};
