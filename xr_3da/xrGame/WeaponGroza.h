#pragma once
#include "weaponmagazinedwgrenade.h"

class CWeaponGroza :
	public CWeaponMagazinedWGrenade
{
	typedef CWeaponMagazinedWGrenade inherited;
public:
	CWeaponGroza(void);
	virtual ~CWeaponGroza(void);

	//virtual void	Load			(LPCSTR section);
	//virtual void	switch2_Idle	();
	//virtual void	switch2_Reload	();
	//virtual void	OnShot			();

	//virtual bool Action(s32 cmd, u32 flags);

	//void SwitchMode();

	//bool m_grenadeMode;
	//MotionSVec		mhud_idle_g;
	//MotionSVec		mhud_reload_g;
	//MotionSVec		mhud_shots_g;
	//MotionSVec		mhud_switch_g, mhud_switch;
};
