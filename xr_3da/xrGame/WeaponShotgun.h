#pragma once
#include "weaponcustompistol.h"

class CWeaponShotgun :	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponShotgun(void);
	virtual ~CWeaponShotgun(void);

	virtual void	Load			(LPCSTR section);
	
	virtual void	net_Destroy			();
	virtual void	Fire2Start			();
	virtual void	Fire2End			();
	virtual void	OnShot				();
	virtual void	OnShotBoth			();
	virtual void	switch2_Fire		();
	virtual void	switch2_Fire2		();
	virtual	void	renderable_Render	();

	virtual void	UpdateSounds	();
	virtual void	UpdateCL		();


	virtual bool Action(s32 cmd, u32 flags);

protected:
	HUD_SOUND		sndShotBoth;
	ESoundTypes		m_eSoundShotBoth;
	MotionSVec		mhud_shot_boths;
};