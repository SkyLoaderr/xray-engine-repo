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

	sound3D			sndFireStart;
	sound3D			sndFireEnd;
	sound3D			sndFireLoop;
	sound3D			sndServo;
	sound3D			sndRicochet[SND_RIC_COUNT];

	vector<Shader*>	hFlames;

	int				iWpnRotBone;
	int				iHUDRotBone;
	int				iFireBone;

	Fvector			vFirePoint;

	Fvector			vLastFP, vLastFD;
	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;
	
	BYTE			bRotAxisHUD;
	BYTE			bRotAxisWpn;

	EM134State		st_current, st_target;

	void			DrawFlame		(const Fvector& fp, const Fvector& fd, bool bHUDView);
	void			UpdateFP		(BOOL bHUD);
	void			UpdateXForm		(BOOL bHUD);

protected:
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
};

#endif //__XR_WEAPON_M134_H__
