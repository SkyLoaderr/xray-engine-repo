#pragma once
#include "weaponcustompistol.h"

class CWeaponPistol :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponPistol(LPCSTR name);
	virtual ~CWeaponPistol(void);

	virtual void	Load			(LPCSTR section);
	virtual void	switch2_Idle	();
	virtual void	switch2_Reload	();
	virtual void	switch2_Hiding	();
	virtual void	OnShot			();
	virtual void	OnAnimationEnd	();
	virtual void	net_Destroy		();
	virtual void	OnH_B_Chield	();

	sound			sndClose;
	ESoundTypes		m_eSoundClose;

	MotionSVec		mhud_empty;
	MotionSVec		mhud_shot_l;
	MotionSVec		mhud_close;

	bool m_opened;
};
