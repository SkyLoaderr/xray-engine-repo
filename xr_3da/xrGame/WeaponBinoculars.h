#ifndef __XR_WEAPON_BINOCULAR_H__
#define __XR_WEAPON_BINOCULAR_H__
#pragma once

#include "WeaponCustomPistol.h"
 
class CWeaponBinoculars: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:
	float			fMaxZoomFactor;
	float			fGyroSpeed;
	// Media :: sounds
	sound			sndZoomIn;
	sound			sndZoomOut;
	sound			sndGyro;
	sound			sndShow;
	sound			sndHide;
	// HUD :: Animations
	MotionSVec		mhud_idle;
	MotionSVec		mhud_hide;
	MotionSVec		mhud_show;

	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	virtual void	UpdateFP		();
	virtual void	UpdateXForm		();
protected:
	enum EState
	{
		eIdle,
		eZooming,
		eShowing,
		eHiding
	};
	EState			st_current, st_target;
	virtual void	switch2_Idle	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Showing	();
	virtual void	switch2_Zooming	();
	virtual void	OnShow			();
	virtual void	OnHide			();
	virtual void	OnZoomIn		();
	virtual void	OnZoomOut		();
	virtual void	OnAnimationEnd	();

	void			state_Zooming	(float dt);
public:
					CWeaponBinoculars(); 
	virtual			~CWeaponBinoculars();

	void			Load			(LPCSTR section);
	virtual	void	Update			(DWORD	dT);
	virtual	void	OnVisible		();

	virtual void	Hide			();
	virtual void	Show			();
	virtual BOOL	HasOpticalAim	()		{ return TRUE; }
	virtual float	GetZoomFactor	();
};

#endif //__XR_WEAPON_BINOCULAR_H__
