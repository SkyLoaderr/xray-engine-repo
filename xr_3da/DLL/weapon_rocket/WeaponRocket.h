// WeaponRail.h: interface for the CWeaponRail class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\\..\\Weapon.h"
//#include "..\\..\\fastsprite.h"

//refs
class CWeaponRocket;
class CFastSprite;

enum ERocketState{
	rcFly,
	rcExplosion,
	rcWaitingDestroy,
	rcDestroy
};


class CRocket //: public CFastSprite
{
	friend class	CWeaponRocket;

	Flight			light;
	FBasicVisual*	pVisual;
	ERocketState	state;
	CWeaponRocket*	pOwner;
	float			fLifeTime;
	float			fTrailTime;
	float			fExplodeTime;
	float			fExplodeAddS;
	Fmatrix			mSelfTransform;
	Fvector			fDir, fPos;

	void			AddTrail	();
public:
					CRocket		(CWeaponRocket* owner,Fvector &dir,Fvector &pos);
	virtual			~CRocket	();
	void			Explode		();
	void			HitIfActor	();
	void			OnMove		();
};

class CWeaponRocket : public CWeapon  
{
	friend class	CRocket;
	float			fTime;
	Fvector			ray_dir;
	vector<CRocket*>Rockets;

	int				hSndFire;
	int				hSndExpl;
public:
	CWeaponRocket				(void *);
	virtual ~CWeaponRocket		();

	void			AddRocket	(Fvector &p, Fvector &d);

	virtual void	SetDefaults	();
	virtual void	FireStart	();
	virtual void	FireEnd		();
	virtual void	OnMove		();
};
