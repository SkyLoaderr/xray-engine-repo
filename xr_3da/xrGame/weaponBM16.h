#pragma once
#include "weaponShotgun.h"

class CWeaponBM16 :public CWeaponShotgun
{
	typedef CWeaponShotgun inherited;
protected:
	MotionSVec		mhud_reload1;
	MotionSVec		mhud_reload2;
	MotionSVec		mhud_shot1;
	MotionSVec		mhud_shot2;
	MotionSVec		mhud_idle_empty;
	MotionSVec		mhud_idle1;
	MotionSVec		mhud_idle2;
	MotionSVec		mhud_idle_zoomed_empty;
	MotionSVec		mhud_zoomed_idle1;
	MotionSVec		mhud_zoomed_idle2;

	MotionSVec		mhud_idlecocks;
	HUD_SOUND		m_sndReload1;
	HUD_SOUND		m_sndReload2;

public:
	virtual			~CWeaponBM16					();
	virtual void	Load							(LPCSTR section);

protected:
	virtual void	PlayAnimShoot					();
	virtual void	PlayAnimReload					();
	virtual void	PlayReloadSound					();
	virtual void	PlayAnimIdle					();
};
