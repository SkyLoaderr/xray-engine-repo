#ifndef __XR_WEAPON_AK74_H__
#define __XR_WEAPON_AK74_H__
#pragma once

#include "WeaponMagazined.h"

#define SND_RIC_COUNT 5
 
class CWeaponAK74: public CWeaponMagazined
{
private:
	typedef CWeaponMagazined inherited;
private:
	// Media :: sounds
	sound3D					sndFireLoop;
	sound3D					sndEmptyClick;
	sound3D					sndReload;
	sound3D					sndRicochet		[SND_RIC_COUNT];

	// Media :: flames
	svector<Shader*,8>		hFlames;
	int						iFlameDiv;
	float					fFlameLength;
	float					fFlameSize;
	
	// HUD :: Animations
	CMotionDef*				mhud_idle;
	CMotionDef*				mhud_reload;
	CMotionDef*				mhud_hide;
	CMotionDef*				mhud_show;
	svector<CMotionDef*,8>	mhud_shots;
protected:
	virtual void	MediaLOAD		();
	virtual void	MediaUNLOAD		();
	virtual void	switch2_Idle	(BOOL bHUDView);
	virtual void	switch2_Fire	(BOOL bHUDView);
	virtual void	switch2_Empty	(BOOL bHUDView);
	virtual void	switch2_Reload	(BOOL bHUDView);
	virtual void	OnShot			(BOOL bHUDView);
	virtual void	OnEmptyClick	(BOOL bHUDView);
	virtual void	OnDrawFlame		(BOOL bHUDView);
	virtual void	OnShotmark		(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
public:
					CWeaponAK74		();
	virtual			~CWeaponAK74	();

	virtual void	Load			(CInifile* ini, const char* section);
	virtual void	Update			(float dt, BOOL bHUDView);

};

#endif //__XR_WEAPON_AK74_H__
