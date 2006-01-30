#ifndef __XR_WEAPON_BINOCULAR_H__
#define __XR_WEAPON_BINOCULAR_H__

#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

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
	float			m_fRTZoomFactor; //run-time zoom factor
	bool			m_bVision;
//	CUIFrameWindow*	UIEntityBorder;
public:
					CWeaponBinoculars	(); 
	virtual			~CWeaponBinoculars	();

	void			Load				(LPCSTR section);

	virtual void	Hide				();
	virtual void	Show				();

	virtual void	OnZoomIn			();
	virtual void	OnZoomOut			();
	virtual	void	ZoomInc				();
	virtual	void	ZoomDec				();
	virtual void	net_Destroy			();
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);


	virtual bool	Action				(s32 cmd, u32 flags);
	virtual void	UpdateCL			();
	virtual void	OnDrawUI			();
protected:
	CBinocularsVision*					m_binoc_vision;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponBinoculars)
#undef script_type_list
#define script_type_list save_type_list(CWeaponBinoculars)

#endif //__XR_WEAPON_BINOCULAR_H__
