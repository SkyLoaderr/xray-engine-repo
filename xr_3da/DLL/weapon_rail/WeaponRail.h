// WeaponRail.h: interface for the CWeaponRail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPONRAIL_H__B8C1C4A4_5CB5_4BAA_A084_5C234ED3716C__INCLUDED_)
#define AFX_WEAPONRAIL_H__B8C1C4A4_5CB5_4BAA_A084_5C234ED3716C__INCLUDED_
#pragma once

#include "..\\..\\Weapon.h"

// refs
class CFastSpriteRail;

class CWeaponRail : public CWeapon  
{
	float				fTime;
	CFastSpriteRail*	pTrail;		

	int					hSndFire;
	bool				bCanFire;
public:
	CWeaponRail					(void *);
	virtual ~CWeaponRail		();

	void			SetRail		(Fvector &p1, Fvector &d, float dist, BOOL bWallmark);
	void			HitIfActor	();

	virtual void	SetDefaults	();
	virtual void	FireStart	();
	virtual void	FireEnd		();
	virtual void	OnMove		();
};

#endif // !defined(AFX_WEAPONRAIL_H__B8C1C4A4_5CB5_4BAA_A084_5C234ED3716C__INCLUDED_)
