#ifndef __XR_WEAPON_BINOCULAR_H__
#define __XR_WEAPON_BINOCULAR_H__
#pragma once

#include "WeaponCustomPistol.h"
 
class CWeaponBinoculars: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
	// Media :: sounds
	HUD_SOUND		sndZoomIn;
	HUD_SOUND		sndZoomOut;
public:
					CWeaponBinoculars(); 
	virtual			~CWeaponBinoculars();

	void			Load			(LPCSTR section);

	virtual void	Hide			();
	virtual void	Show			();

	virtual void	OnZoomIn		();
	virtual void	OnZoomOut		();


	virtual bool	Action			(s32 cmd, u32 flags);
};

#endif //__XR_WEAPON_BINOCULAR_H__
