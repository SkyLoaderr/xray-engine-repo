#ifndef __XR_WEAPON_PROTECTA_H__
#define __XR_WEAPON_PROTECTA_H__
#pragma once

#include "weapon.h"

class ENGINE_API C3DSound;

#define SND_RIC_COUNT 5

class CWeaponProtecta: public CWeapon
{
	enum EGrozaState
	{
		eIdle,
		eReload,
		eFire
	};
	typedef CWeapon inherited;

private:
	// General
	float			fTime;

	sound3D			sndFire;
	sound3D			sndRicochet[SND_RIC_COUNT];

	vector<Shader*>	hFlames;

	Fvector			vFirePoint;

	Fvector			vLastFP, vLastFD;
	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;

	int				iShotCount;
	
	EGrozaState		st_current, st_target;

	void			DrawFlame		(const Fvector& fp, const Fvector& fd, bool bHUDView);
	void			UpdateFP		(BOOL bHUD);
	void			UpdateXForm		(BOOL bHUD);

protected:
	virtual void	AddShotmark		(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
public:
					CWeaponProtecta	();
	virtual			~CWeaponProtecta();

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

#endif //__XR_WEAPON_PROTECTA_H__
