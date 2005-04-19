#ifndef __XR_WEAPON_BINOCULAR_H__
#define __XR_WEAPON_BINOCULAR_H__
#pragma once

#include "WeaponCustomPistol.h"
class CUIFrameWindow;
class CUIStatic;
class CBinocularsVision;

class CWeaponBinoculars: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
	// Media :: sounds
	HUD_SOUND		sndZoomIn;
	HUD_SOUND		sndZoomOut;
//	CUIFrameWindow*	UIEntityBorder;
public:
					CWeaponBinoculars	(); 
	virtual			~CWeaponBinoculars	();

	void			Load				(LPCSTR section);

	virtual void	Hide				();
	virtual void	Show				();

	virtual void	OnZoomIn			();
	virtual void	OnZoomOut			();
	virtual void	net_Destroy			();


	virtual bool	Action				(s32 cmd, u32 flags);
	virtual void	UpdateCL			();
	virtual void	OnDrawUI			();
protected:
	CBinocularsVision*					m_binoc_vision;
};

#endif //__XR_WEAPON_BINOCULAR_H__
