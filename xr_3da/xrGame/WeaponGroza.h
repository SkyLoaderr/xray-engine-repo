#ifndef __XR_WEAPON_GROZA_H__
#define __XR_WEAPON_GROZA_H__
#pragma once

#include "weapon.h"

class ENGINE_API C3DSound;

#define SND_RIC_COUNT 5

class CWeaponGroza: public CWeapon
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
	BOOL			bFlame;

	sound3D			sndFireLoop;
	sound3D			sndRicochet[SND_RIC_COUNT];

	vector<Shader*>	hFlames;

	Fvector			vFirePoint;

	Fvector			vLastFP, vLastFD;
	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;
	
	EGrozaState		st_current, st_target;

	void			DrawFlame		(const Fvector& fp, const Fvector& fd, bool bHUDView);
	void			UpdateFP		(BOOL bHUD);
	void			UpdateXForm		(BOOL bHUD);

protected:
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
public:
					CWeaponGroza	();
	virtual			~CWeaponGroza	();

	// misc
	virtual void	Load			(CInifile* ini, const char* section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual BOOL	HasOpticalAim	()				{ return TRUE; }

	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);
};

#endif //__XR_WEAPON_GROZA_H__
