#pragma once
#include "weaponcustompistol.h"

class CWeaponSVU :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
	float			fMaxZoomFactor;
public:
	CWeaponSVU(void);
	virtual ~CWeaponSVU(void);
	virtual void	Load			(LPCSTR section);
	virtual void			Fire2Start			();
	virtual void			Fire2End			();
};
