#ifndef __XR_WEAPON_MAG_H__
#define __XR_WEAPON_MAG_H__
#pragma once

#include "weapon.h"

class ENGINE_API CMotionDef;

#define SND_RIC_COUNT 5
#define MAX_ANIM_COUNT 8
 
typedef svector<CMotionDef*,MAX_ANIM_COUNT> MotionSVec;

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

	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;
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
	EState			st_current, st_target;
	void			animGet			(MotionSVec& lst, LPCSTR prefix);
protected:
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
	virtual void	OnMagazineEmpty	();

	virtual void	MediaLOAD		();
	virtual void	MediaUNLOAD		();
	virtual void	switch2_Idle	(BOOL bHUD);
	virtual void	switch2_Fire	(BOOL bHUD);
	virtual void	switch2_Empty	(BOOL bHUD);
	virtual void	switch2_Reload	(BOOL bHUD);
	virtual void	switch2_Hiding	(BOOL bHUD);
	virtual void	switch2_Showing	(BOOL bHUD);
	virtual void	OnShot			(BOOL bHUD);
	virtual void	OnShellDrop		(BOOL bHUD);
	virtual void	OnEmptyClick	(BOOL bHUD);
	virtual void	OnShotmark		(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
	virtual void	OnShow			();
	virtual void	OnHide			();
	virtual void	OnAnimationEnd	();
	void			TryReload		();
protected:
	virtual void	UpdateFP		(BOOL bHUD);
	virtual void	UpdateXForm		(BOOL bHUD);
	virtual void	ReloadMagazine	();

	virtual void	state_Fire		(BOOL bHUD, float dt);
	virtual void	state_MagEmpty	(BOOL bHUD, float dt);
public:
					CWeaponMagazined	(LPCSTR name);
	virtual			~CWeaponMagazined	();

	virtual void	Load			(CInifile* ini, const char* section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual void	Reload			();
	
	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);

	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif //__XR_WEAPON_MAG_H__
