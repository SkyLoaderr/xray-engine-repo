#include "stdafx.h"
#include "WeaponBinoculars.h"

#include "xr_level_controller.h"

#include "level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponBinoculars::CWeaponBinoculars() : CWeaponCustomPistol("BINOCULARS")
{
}

CWeaponBinoculars::~CWeaponBinoculars()
{
	HUD_SOUND::DestroySound(sndZoomIn);
	HUD_SOUND::DestroySound(sndZoomOut);
}

void CWeaponBinoculars::Load	(LPCSTR section)
{
	inherited::Load(section);

	// Sounds
	HUD_SOUND::LoadSound(section, "snd_zoomin",  sndZoomIn,		TRUE, SOUND_TYPE_ITEM_USING);
	HUD_SOUND::LoadSound(section, "snd_zoomout", sndZoomOut,	TRUE, SOUND_TYPE_ITEM_USING);
}

void CWeaponBinoculars::Hide		()
{
	SwitchState(eHidden);
}
void CWeaponBinoculars::Show		()
{
	SwitchState(eShowing);
}

bool CWeaponBinoculars::Action(s32 cmd, u32 flags) 
{
	switch(cmd) 
	{
	case kWPN_FIRE : 
		return inherited::Action(kWPN_ZOOM, flags);
	}

	return inherited::Action(cmd, flags);
}

void CWeaponBinoculars::OnZoomIn		()
{
	if(H_Parent() && !IsZoomed())
	{
		HUD_SOUND::StopSound(sndZoomOut);
		bool hud_mode = (Level().CurrentEntity() == H_Parent());
		HUD_SOUND::PlaySound(sndZoomIn, H_Parent()->Position(), H_Parent(), hud_mode);
	}

	inherited::OnZoomIn();
}
void CWeaponBinoculars::OnZoomOut		()
{
	if(H_Parent() && IsZoomed() && !IsRotatingToZoom())
	{
		HUD_SOUND::StopSound(sndZoomIn);
		bool hud_mode = (Level().CurrentEntity() == H_Parent());	
		HUD_SOUND::PlaySound(sndZoomOut, H_Parent()->Position(), H_Parent(), hud_mode);
	}

	inherited::OnZoomOut();
}

