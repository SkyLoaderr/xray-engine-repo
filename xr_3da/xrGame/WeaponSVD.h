#pragma once
#include "weaponcustompistol.h"

class CWeaponSVD :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
	float			fMaxZoomFactor;
public:
	CWeaponSVD(void);
	~CWeaponSVD(void);
	virtual void	Load			(LPCSTR section);
	virtual void			Fire2Start			();
	virtual void			Fire2End			();
};
