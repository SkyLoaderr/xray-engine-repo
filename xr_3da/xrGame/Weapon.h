// Weapon.h: interface for the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
#define AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_
#pragma once

#include "..\xr_collide_defs.h"

// refs
class CEntity;
class CWeaponHUD;

class CWeapon : public CObject
{
	friend class	CWeaponList;
	BOOL			bWorking;
	char*			m_WpnName;
protected:
	CEntity*		m_pParent;
	CWeaponList*	m_pContainer;
	CWeaponHUD*		m_pHUD;

	Shader*			hWallmark;
	float			fWallmarkSize;

	int				iAmmoLimit;
	int				iAmmoElapsed;
	float			fTimeToFire;
	int				iHitPower;

	float			fireDistance;
	float			fireDispersion;

	BOOL			FireTrace		(Fvector& P, Fvector& D, Collide::ray_query& R);

	void			UpdatePosition	(const Fmatrix& transform);

	virtual void	AddShotmark		(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R);
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
	
	virtual void	Hide			()				{};
	virtual void	Show			()				{};

	BOOL			IsWorking		()				{ return bWorking;	}
	BOOL			IsValid			()				{ return (iAmmoElapsed!=0); }

	virtual	void	Update			(float dt, BOOL bHUDView)	{};
	virtual	void	Render			(BOOL bHUDView)				{};

	IC const char*	GetName			(){return m_WpnName;}
	IC int			GetAmmoElapsed	(){return iAmmoElapsed;}
	IC int			GetAmmoLimit	(){return iAmmoLimit;}
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
