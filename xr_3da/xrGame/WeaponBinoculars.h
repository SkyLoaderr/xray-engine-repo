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
protected:
	enum EState
	{
		eZooming	= 128
	};

	virtual void	switch2_Idle	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Hidden	();
	virtual void	switch2_Showing	();
	virtual void	switch2_Zooming	();

	virtual void	OnZoomIn		();
	virtual void	OnZoomOut		();
	virtual void	OnAnimationEnd	();
	virtual void	OnStateSwitch	(u32 S);

	void			state_Zooming	(float dt);
public:
					CWeaponBinoculars(); 
	virtual			~CWeaponBinoculars();

	void			Load			(LPCSTR section);
	virtual	void	UpdateCL		();
	virtual	void	renderable_Render		();

	virtual void	Hide			();
	virtual void	Show			();
	virtual BOOL	HasOpticalAim	()		{ return TRUE; }
	virtual float	GetZoomFactor	();
	virtual void			Fire2Start			();
	virtual void			Fire2End			();

	virtual const char* Name();
	virtual bool Action(s32 cmd, u32 flags);
};

#endif //__XR_WEAPON_BINOCULAR_H__
