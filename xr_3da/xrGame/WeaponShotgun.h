#pragma once
#include "weaponcustompistol.h"

class CWeaponShotgun :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponShotgun(void);
	~CWeaponShotgun(void);

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
	virtual	void	OnVisible		();
	virtual BOOL			FireTrace			(const Fvector& P, const Fvector& Peff,	Fvector& D);

	Fvector					vFirePoint2;
	sound			sndShotBoth;
	ESoundTypes		m_eSoundShotBoth;
	MotionSVec		mhud_shot_boths;
};
