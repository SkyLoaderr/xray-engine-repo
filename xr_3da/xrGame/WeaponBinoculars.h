#ifndef __XR_WEAPON_BINOCULAR_H__
#define __XR_WEAPON_BINOCULAR_H__
#pragma once

#include "WeaponCustomPistol.h"
 
class CWeaponBinoculars: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:
	// Media :: sounds
	sound			sndIdle;
	sound			sndShow;
	sound			sndHide;
	// HUD :: Animations
	MotionSVec		mhud_idle;
	MotionSVec		mhud_hide;
	MotionSVec		mhud_show;

	DWORD			dwXF_Frame;

	virtual void	UpdateFP		(BOOL bHUD){;}
	virtual void	UpdateXForm		(BOOL bHUD);
protected:
	enum EState
	{
		eIdle,
		eShowing,
		eHiding
	};
	EState			st_current, st_target;
	virtual void	switch2_Idle	(BOOL bHUD);
	virtual void	switch2_Hiding	(BOOL bHUD);
	virtual void	switch2_Showing	(BOOL bHUD);
	virtual void	OnShow			();
	virtual void	OnHide			();
	virtual void	OnAnimationEnd	();
public:
					CWeaponBinoculars(); 
	virtual			~CWeaponBinoculars();
	virtual BOOL	HasOpticalAim	()		{ return TRUE; }
	void			Load			(CInifile* ini, const char* section);

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);

	virtual void	Hide			();
	virtual void	Show			();
};

#endif //__XR_WEAPON_BINOCULAR_H__
