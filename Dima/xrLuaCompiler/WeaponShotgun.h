#pragma once
#include "weaponcustompistol.h"

class CWeaponShotgun :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponShotgun(void);
	virtual ~CWeaponShotgun(void);

	virtual void	Load			(LPCSTR section);
	//virtual void OnZoomIn();
	virtual void	net_Destroy		();
	virtual void			Fire2Start			();
	virtual void			Fire2End			();
	virtual void			OnShot			();
	virtual void	OnShotBoth			();
	virtual void	switch2_Fire	();
	virtual void	switch2_Fire2	();
	virtual void			OnDrawFlame			();
	virtual	void	renderable_Render		();
	virtual BOOL			FireTrace2			(const Fvector& P, const Fvector& Peff,	Fvector& D);
	virtual bool Action(s32 cmd, u32 flags);

	Fvector					vFirePoint2;
	ref_sound			sndShotBoth;
	ESoundTypes		m_eSoundShotBoth;
	MotionSVec		mhud_shot_boths;
};
