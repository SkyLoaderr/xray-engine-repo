#ifndef __XR_WEAPON_MAG_H__
#define __XR_WEAPON_MAG_H__
#pragma once

#include "weapon.h"

class ENGINE_API CMotionDef;

#define SND_RIC_COUNT 5

class CWeaponMagazined: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:
	// Media :: sounds
	sound			sndShow;
	sound			sndHide;
	sound			sndShot;
	sound			sndEmptyClick;
	sound			sndReload;
	sound			sndRicochet		[SND_RIC_COUNT];

	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;
	ESoundTypes		m_eSoundShot;
	ESoundTypes		m_eSoundEmptyClick;
	ESoundTypes		m_eSoundReload;
	ESoundTypes		m_eSoundRicochet;
					
	// HUD :: Animations
	MotionSVec		mhud_idle;
	MotionSVec		mhud_reload;
	MotionSVec		mhud_hide;
	MotionSVec		mhud_show;
	MotionSVec		mhud_shots;
	// General
	float			fTime;
	BOOL			bFlame;
protected:
	enum EState
	{
		eIdle,
		eFire,
		eFrozen,	// for pistols, etc
		eMagEmpty,
		eReload,
		eShowing,
		eHiding
	};
protected:
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
	virtual void	OnMagazineEmpty	();

	virtual void	MediaLOAD		();
	virtual void	MediaUNLOAD		();
	virtual void	switch2_Idle	();
	virtual void	switch2_Fire	();
	virtual void	switch2_Empty	();
	virtual void	switch2_Reload	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Showing	();
	virtual void	OnShot			();
	virtual void	OnShellDrop		();
	virtual void	OnEmptyClick	();
	virtual void	OnShotmark		(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
	virtual void	OnShow			();
	virtual void	OnHide			();
	virtual void	OnAnimationEnd	();
	virtual void	OnStateSwitch	(DWORD S);

	void			TryReload		();
protected:
	virtual void	ReloadMagazine	();

	virtual void	state_Fire		(float dt);
	virtual void	state_MagEmpty	(float dt);
public:
					CWeaponMagazined	(LPCSTR name,ESoundTypes eSoundType);
	virtual			~CWeaponMagazined	();

	virtual void	Load			(LPCSTR section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual void	Reload			();
	
	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	UpdateCL		();
	virtual	void	OnVisible		();

	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif //__XR_WEAPON_MAG_H__
