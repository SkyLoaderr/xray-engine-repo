#ifndef __XR_WEAPON_AK74_H__
#define __XR_WEAPON_AK74_H__
#pragma once

#include "weapon.h"

#define SND_RIC_COUNT 5
 
class CWeaponAK74: public CWeapon
{
	enum EAK74State
	{
		eIdle,
		eFire,
		eMagazineEmpty,
		eReload
	};
	typedef CWeapon inherited;

private:
	// General
	float			fTime;
	BOOL			bFlame;

	sound3D			sndFireLoop;
	sound3D			sndRicochet		[SND_RIC_COUNT];
	sound3D			sndEmptyClick;

	vector<Shader*>	hFlames;

	Fvector			vFirePoint;

	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;
	
	EAK74State		st_current, st_target;

	void			DrawFlame		(const Fvector& fp, const Fvector& fd, bool bHUDView);
	void			FlameLOAD		();
	void			FlameUNLOAD		();
	
protected:
	virtual void	UpdateFP		(BOOL bHUD);
	virtual void	UpdateXForm		(BOOL bHUD);
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
	virtual void	OnMagazineEmpty	();
public:
					CWeaponAK74		();
	virtual			~CWeaponAK74	();

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

#endif //__XR_WEAPON_AK74_H__
