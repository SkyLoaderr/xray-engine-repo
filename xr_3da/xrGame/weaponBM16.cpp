#include "stdafx.h"
#include "weaponBM16.h"
#include "WeaponHUD.h"

CWeaponBM16::~CWeaponBM16()
{
	HUD_SOUND::DestroySound(m_sndReload1);
	HUD_SOUND::DestroySound(m_sndReload2);
}

void CWeaponBM16::Load	(LPCSTR section)
{
	inherited::Load		(section);

	animGet	(mhud_reload1,	pSettings->r_string(*hud_sect,"anim_reload_1"));
	animGet	(mhud_reload2,	pSettings->r_string(*hud_sect,"anim_reload_2"));
	animGet	(mhud_shot1,	pSettings->r_string(*hud_sect,"anim_shoot_1"));
	animGet	(mhud_shot2,	pSettings->r_string(*hud_sect,"anim_shoot_2"));

	animGet	(mhud_idle_empty,	pSettings->r_string(*hud_sect,"anim_idle_empty"));
	animGet	(mhud_idle1,		pSettings->r_string(*hud_sect,"anim_idle_1"));
	animGet	(mhud_idle2,		pSettings->r_string(*hud_sect,"anim_idle_2"));

	animGet	(mhud_idle_zoomed_empty,	pSettings->r_string(*hud_sect,"anim_zoomed_idle_empty"));
	animGet	(mhud_zoomed_idle1,		pSettings->r_string(*hud_sect,"anim_zoomed_idle_1"));
	animGet	(mhud_zoomed_idle2,		pSettings->r_string(*hud_sect,"anim_zoomedidle_2"));


	HUD_SOUND::LoadSound(section, "snd_reload_1", m_sndReload1, TRUE, m_eSoundShot);
	HUD_SOUND::LoadSound(section, "snd_reload_2", m_sndReload2, TRUE, m_eSoundShot);

}

void CWeaponBM16::PlayReloadSound()
{
	if(m_magazine.size()==1)
		PlaySound	(m_sndReload1,vLastFP);
	else
		PlaySound	(m_sndReload2,vLastFP);
}

void CWeaponBM16::PlayAnimShoot()
{
	if(m_magazine.size()==1)
		m_pHUD->animPlay(mhud_shot1[Random.randI(mhud_shot1.size())],TRUE,this);
	else
		m_pHUD->animPlay(mhud_shot2[Random.randI(mhud_shot2.size())],TRUE,this);
}

void CWeaponBM16::PlayAnimReload()
{
	if(m_magazine.size()==1)
		m_pHUD->animPlay(mhud_reload1[Random.randI(mhud_reload1.size())],TRUE,this);
	else
		m_pHUD->animPlay(mhud_reload2[Random.randI(mhud_reload2.size())],TRUE,this);

}

void CWeaponBM16::PlayAnimIdle()
{
	if(IsZoomed())
	{
		switch (m_magazine.size())
		{
		case 0:{
			m_pHUD->animPlay(mhud_idle_zoomed_empty[Random.randI(mhud_idle_zoomed_empty.size())], TRUE);
		}break;
		case 1:{
			m_pHUD->animPlay(mhud_zoomed_idle1[Random.randI(mhud_zoomed_idle1.size())], TRUE);
		}break;
		case 2:{
			m_pHUD->animPlay(mhud_zoomed_idle2[Random.randI(mhud_zoomed_idle2.size())], TRUE);
		}break;
		};
	}else{
		switch (m_magazine.size())
		{
		case 0:{
			m_pHUD->animPlay(mhud_idle_empty[Random.randI(mhud_idle_empty.size())], TRUE);
		}break;
		case 1:{
			m_pHUD->animPlay(mhud_idle1[Random.randI(mhud_idle1.size())], TRUE);
		}break;
		case 2:{
			m_pHUD->animPlay(mhud_idle2[Random.randI(mhud_idle2.size())], TRUE);
		}break;
		};
	}
	

}
