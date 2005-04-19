#include "stdafx.h"
#include "WeaponBinoculars.h"

#include "xr_level_controller.h"

#include "level.h"
#include "ui\UIFrameWindow.h"
#include "WeaponBinocularsVision.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponBinoculars::CWeaponBinoculars() : CWeaponCustomPistol("BINOCULARS")
{
	m_binoc_vision	= NULL;
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
	inherited::Hide();
}

void CWeaponBinoculars::Show		()
{
	inherited::Show();
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
		m_binoc_vision = xr_new<CBinocularsVision>(this);
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
		xr_delete(m_binoc_vision);
	}

	inherited::OnZoomOut();
}
void	CWeaponBinoculars::net_Destroy()
{
	inherited::net_Destroy();
	xr_delete(m_binoc_vision);
}

void	CWeaponBinoculars::UpdateCL()
{
	inherited::UpdateCL();
//manage visible entities here...
	if(H_Parent() && IsZoomed() && !IsRotatingToZoom() && m_binoc_vision)
		m_binoc_vision->Update();
}

void CWeaponBinoculars::OnDrawUI()
{
	if(H_Parent() && IsZoomed() && !IsRotatingToZoom() && m_binoc_vision)
		m_binoc_vision->Draw();
/*		UIEntityBorder->SetWndPos		(500,500);
		UIEntityBorder->SetWidth		(100);
		UIEntityBorder->SetHeight		(100);
		UIEntityBorder->Draw			();*/
	inherited::OnDrawUI	();
}
