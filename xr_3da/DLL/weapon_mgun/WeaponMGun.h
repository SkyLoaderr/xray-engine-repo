// WeaponMGun.h: interface for the CWeaponMGun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPONMGUN_H__18DCD12B_2042_4456_A900_6B8F63061C30__INCLUDED_)
#define AFX_WEAPONMGUN_H__18DCD12B_2042_4456_A900_6B8F63061C30__INCLUDED_
#pragma once

#include "..\\..\\weapon.h"
#include "..\\..\\fastsprite.h"

//refs
class FShotMarkVisual;

class CWeaponMGun : public CWeapon //, public CFastSprite
{
	float						fTime;
	vector<FShotMarkVisual*>	shotmarks;

	int							hSndFire;
public:
	CWeaponMGun(void *p);
	virtual ~CWeaponMGun();

	void			AddShotmark	(Fvector &p1, Fvector &d, float dist);
	void			HitIfActor	();

	virtual void	SetDefaults	();
	virtual void	FireStart	();
	virtual void	FireEnd		();
	virtual void	OnMove		();
};

#endif // !defined(AFX_WEAPONMGUN_H__18DCD12B_2042_4456_A900_6B8F63061C30__INCLUDED_)
