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
	m_bVision		= false;
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
	m_bVision = !!pSettings->r_bool(section,"vision_present");
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
		if(m_bVision)
			m_binoc_vision = xr_new<CBinocularsVision>(this);
	}

	inherited::OnZoomIn();
	m_fZoomFactor = m_fRTZoomFactor;
}

void CWeaponBinoculars::OnZoomOut		()
{
	if(H_Parent() && IsZoomed() && !IsRotatingToZoom())
	{
		HUD_SOUND::StopSound(sndZoomIn);
		bool hud_mode = (Level().CurrentEntity() == H_Parent());	
		HUD_SOUND::PlaySound(sndZoomOut, H_Parent()->Position(), H_Parent(), hud_mode);
		xr_delete(m_binoc_vision);
	
		m_fRTZoomFactor = m_fZoomFactor;//store current
	}
	inherited::OnZoomOut();
}

BOOL	CWeaponBinoculars::net_Spawn			(CSE_Abstract* DC)
{
	inherited::net_Spawn(DC);
	m_fRTZoomFactor = m_fScopeZoomFactor;
	return TRUE;
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

void GetZoomData(const float scope_factor, float& delta, float& min_zoom_factor)
{
	float def_fov = float(DEFAULT_FOV);
	float min_zoom_k = 0.3f;
	float zoom_step_count = 3.0f;
	float delta_factor_total = def_fov-scope_factor;
	VERIFY(delta_factor_total>0);
	min_zoom_factor = def_fov-delta_factor_total*min_zoom_k;
	delta = (delta_factor_total*(1-min_zoom_k) )/zoom_step_count;

}

void CWeaponBinoculars::ZoomInc()
{
	float delta,min_zoom_factor;
	GetZoomData(m_fScopeZoomFactor,delta,min_zoom_factor);

	m_fZoomFactor	-=delta;
	clamp(m_fZoomFactor,m_fScopeZoomFactor,min_zoom_factor);
}

void CWeaponBinoculars::ZoomDec()
{
	float delta,min_zoom_factor;
	GetZoomData(m_fScopeZoomFactor,delta,min_zoom_factor);

	m_fZoomFactor	+=delta;
	clamp(m_fZoomFactor,m_fScopeZoomFactor, min_zoom_factor);
}