#ifndef __XR_WEAPON_M134_H__
#define __XR_WEAPON_M134_H__
#pragma once

#include "weapon.h"

class ENGINE_API C3DSound;

#define SND_RIC_COUNT 5

class CWeaponM134 : public CWeapon
{
	enum EM134State
	{
		eM134Idle,
		eM134Spinup,
		eM134Brake,
		eM134Fire
	};
	typedef CWeapon inherited;

	static void __stdcall RotateCallback_norm	(CBoneInstance* B);
	static void __stdcall RotateCallback_hud	(CBoneInstance* B);
private:
	// General
	float			fTime;
	float			fRotateSpeed;
	float			fRotateAngle;

	float			fRotateMaxSpeed;
	float			fRotateSpinupAccel;
	float			fRotateBreakAccel;

	DWORD			dwServoMaxFreq;
	DWORD			dwServoMinFreq;

	sound			sndFireStart;
	sound			sndFireEnd;
	sound			sndFireLoop;
	sound			sndServo;
	sound			sndRicochet[SND_RIC_COUNT];

	vector<Shader*>	hFlames;

	int				iWpnRotBone;
	int				iHUDRotBone;
	int				iFireBone;

	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;
	
	BYTE			bRotAxisHUD;
	BYTE			bRotAxisWpn;

	EM134State		st_current, st_target;

	CMotionDef*		mhud_idle;	
	CMotionDef*		mhud_fire;	
	CMotionDef*		mhud_spinup;
	
	void			DrawFlame		(const Fvector& fp, const Fvector& fd, bool bHUDView);
	virtual void	UpdateFP		(BOOL bHUD);
	virtual void	UpdateXForm		(BOOL bHUD);
protected:
	void			FlameLOAD		();
	void			FlameUNLOAD		();
	
	virtual void	FireShotmark	(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R);
public:
					CWeaponM134		();
	virtual			~CWeaponM134	();

	// misc
	virtual void	Load			(CInifile* ini, const char* section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();

	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);
	
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif //__XR_WEAPON_M134_H__
