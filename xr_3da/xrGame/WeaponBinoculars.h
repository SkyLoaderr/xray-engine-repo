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
	ref_sound		sndZoomIn;
	ref_sound		sndZoomOut;
	ref_sound		sndGyro;
	ref_sound		sndShow;
	ref_sound		sndHide;

	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;

public:
					CWeaponBinoculars(); 
	virtual			~CWeaponBinoculars();

	void			Load			(LPCSTR section);

	virtual void	Hide			();
	virtual void	Show			();

	virtual bool	Action			(s32 cmd, u32 flags);
};

#endif //__XR_WEAPON_BINOCULAR_H__
