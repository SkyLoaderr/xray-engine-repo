#ifndef __XR_WEAPON_EMSRIFLE_H__
#define __XR_WEAPON_EMSRIFLE_H__
#pragma once

#include "weapon.h"

class ENGINE_API C3DSound;

#define SND_RIC_COUNT 5

class CWeaponEMSRifle: public CWeapon
{
	enum EEMSRifleState
	{
		eIdle,
		eReload,
		eFire
	};
	typedef CWeapon inherited;

private:
	// General
	float			fTime;

	sound3D			sndFireLoop;
	sound3D			sndRicochet[SND_RIC_COUNT];

	Shader*			hTrail;
	Shader*			hFlame[5];

	int				iWpnFireBone;
	int				iHUDFireBone;

	Fvector			vWpnFirePoint;
	Fvector			vHUDFirePoint;

	Fvector			vLastFP, vLastFD;
	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;
	
	EEMSRifleState	st_current, st_target;

	void			DrawFlame	(const Fvector& fp, const Fvector& fd, bool bHUDView);
	void			UpdateFP	(BOOL bHUD);
	void			UpdateXForm	(BOOL bHUD);

protected:
	virtual void	AddShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
public:
					CWeaponEMSRifle	();
	virtual			~CWeaponEMSRifle();

	// misc
	virtual void	Load		(CInifile* ini, const char* section);

	virtual void	SetDefaults	();
	virtual void	FireStart	();
	virtual void	FireEnd		();
	virtual	void	Update		(float dt, BOOL bHUDView);
	virtual	void	Render		(BOOL bHUDView);
};

#endif //__XR_WEAPON_EMSRIFLE_H__
