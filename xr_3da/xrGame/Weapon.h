// Weapon.h: interface for the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
#define AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_
#pragma once

#include "..\xr_collide_defs.h"
#include "..\lightPPA.h"

// refs
class CEntity;
class CWeaponHUD;
class ENGINE_API CMotionDef;

#define MAX_ANIM_COUNT 8
 
typedef svector<CMotionDef*,MAX_ANIM_COUNT> MotionSVec;

class CWeapon : public CObject
{
	friend class	CWeaponList;
protected:
	BOOL			bWorking;		// Weapon fires now
	BOOL			bPending;		// Weapon needs some time to update itself, even if hidden
	LPSTR			m_WpnName;

	Fmatrix			m_Offset;

	void			signal_HideComplete		();

	Fvector			vFirePoint;
	Fvector			vShellPoint;

	// Media :: flames
	svector<Shader*,8>hFlames;
	int				iFlameDiv;
	float			fFlameLength;
	float			fFlameSize;
	float			fFlameTime;

	void			animGet			(MotionSVec& lst, LPCSTR prefix);
public:
	float			GetPrecision();
protected:
	CWeaponList*	m_pContainer;
	CWeaponHUD*		m_pHUD;

	LPSTR			pstrWallmark;
	Shader*			hWallmark;
	Shader*			hUIIcon;
	float			fWallmarkSize;

	int				iAmmoLimit;			// maximum ammo we can have
	int				iAmmoCurrent;		// ammo we have now
	int				iAmmoElapsed;		// ammo in magazine, currently
	int				iMagazineSize;		// size (in bullets) of magazine

	float			fTimeToFire;
	int				iHitPower;

	Fvector			vLastFP, vLastFD, vLastSP;
	
	float			fireDistance;
	float			fireDispersionBase;
	float			fireDispersion;
	float			fireDispersion_Inc;
	float			fireDispersion_Dec;
	float			fireDispersion_Current;

	float			camMaxAngle;
	float			camRelaxSpeed;
	float			camDispersion;

	float			dispVelFactor;
	float			dispJumpFactor;
	float			dispCrouchFactor;
	
	float			tracerHeadSpeed;
	float			tracerTrailCoeff;
	float			tracerStartLength;
	float			tracerWidth;
	
	CLightPPA		light_base;
	CLightPPA		light_build;
	CLightPPA		light_render;
	float			light_var_color;
	float			light_var_range;
	float			light_lifetime;
	DWORD			light_frame;
	float			light_time;
protected:
	void			Light_Start		();
	void			Light_Render	(Fvector& P);

	virtual BOOL	FireTrace		(const Fvector& P, const Fvector& Peff,	Fvector& D);
	virtual void	FireShotmark	(const Fvector& vDir,	const Fvector &vEnd, Collide::ray_query& R);
	virtual void	UpdatePosition	(const Fmatrix& transform);

	virtual void	UpdateFP		(BOOL bHUD)	= 0;
	virtual void	UpdateXForm		(BOOL bHUD)	= 0;

protected:			// Utilities
	void			SoundCreate		(sound&		dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void			SoundDestroy	(sound&		dest);
	void			ShaderCreate	(Shader*	&dest, LPCSTR S, LPCSTR T);
	void			ShaderDestroy	(Shader*	&dest);
public:
	virtual void	OnMagazineEmpty	()			{};
	virtual void	OnAnimationEnd	()			{};
	virtual void	OnHide			()			{};
	virtual void	OnShow			()			{};
	virtual void	OnZoomIn		()			{};
	virtual void	OnZoomOut		()			{};
public:
					CWeapon			(LPCSTR name);
	virtual			~CWeapon		();

	// misc
	virtual void	Load			(CInifile* ini, const char* section);

	// logic & effects
	virtual void	SetParent		(CEntity* parent, CWeaponList* container);
	virtual void	SetDefaults		();
	virtual bool	AddAmmo			(int iValue)	{ iAmmoElapsed+=iValue; return true;}
	virtual void	FireStart		()				{ bWorking=true;	}
	virtual void	FireEnd			()				{ bWorking=false;	}
	virtual void	Reload			()				{};
	
	virtual void	Hide			();
	virtual void	Show			();

	IC BOOL			IsWorking		()				{ return bWorking;							}
	IC BOOL			IsValid			()				{ return iAmmoCurrent || iAmmoElapsed;		}
	IC BOOL			IsVisible		()				{ return bVisible;							}	// Weapon change occur only after visibility change
	IC BOOL			IsUpdating		()				{ return bWorking || bPending || bVisible;	}	// Does weapon need's update?
	virtual BOOL	HasOpticalAim	()				{ return FALSE; }
	virtual float	GetZoomFactor	()				{ return DEFAULT_FOV; }

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);
	virtual void	OnDrawFlame		(BOOL bHUDView);


	IC LPCSTR		GetName			()				{return m_WpnName;		}
	IC int			GetAmmoElapsed	()				{return iAmmoElapsed;	}
	IC int			GetAmmoLimit	()				{return iAmmoLimit;		}
	IC int			GetAmmoCurrent	()				{return iAmmoCurrent;	}
	IC int			GetAmmoMagSize	()				{return iMagazineSize;	}
	IC Shader*		GetUIIcon		()				{return hUIIcon;		}
	
	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
