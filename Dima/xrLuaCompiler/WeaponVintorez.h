#pragma once
#include "weaponmagazined.h"

class CWeaponVintorez :
	public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;
	float			fMaxZoomFactor;
public:
	CWeaponVintorez(void);
	virtual ~CWeaponVintorez(void);

	virtual void	Load			(LPCSTR section);
	virtual void			Fire2Start			();
	virtual void			Fire2End			();

	virtual bool Action(s32 cmd, u32 flags);
	virtual void			OnDrawFlame			(){}
};
