#include "stdafx.h"
#include "weaponpistol.h"
#include "WeaponHUD.h"
#include "ParticlesObject.h"

CWeaponPistol::CWeaponPistol(LPCSTR name) : CWeaponCustomPistol(name)
{
	m_eSoundClose		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING /*| eSoundType*/);
	m_opened = false;
	m_bPending = false;
}

CWeaponPistol::~CWeaponPistol(void)
{
}

void CWeaponPistol::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	HUD_SOUND::DestroySound(sndClose);
}


void CWeaponPistol::Load	(LPCSTR section)
{
	inherited::Load		(section);

	HUD_SOUND::LoadSound(section, "snd_close", sndClose, TRUE, m_eSoundClose);

	animGet				(mhud_empty,		pSettings->r_string(*hud_sect, "anim_empty"));
	animGet				(mhud_shot_l,		pSettings->r_string(*hud_sect, "anim_shot_last"));
	animGet				(mhud_close,		pSettings->r_string(*hud_sect, "anim_close"));
	animGet				(mhud_show_empty,	pSettings->r_string(*hud_sect, "anim_draw_empty"));
	animGet				(mhud_reload_empty,	pSettings->r_string(*hud_sect, "anim_reload_empty"));
}

void CWeaponPistol::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	m_opened = false;
}

void CWeaponPistol::PlayAnimShow	()
{
	if(iAmmoElapsed > 1)
		m_opened = false;
	else
		m_opened = true;
		
	if(m_opened) 
		m_pHUD->animPlay(mhud_show_empty[Random.randI(mhud_show_empty.size())],FALSE, this);
	else 
		m_pHUD->animPlay(mhud_show[Random.randI(mhud_show.size())],FALSE, this);
}

void CWeaponPistol::PlayAnimIdle	()
{
	if(m_opened) 
		m_pHUD->animPlay(mhud_empty[Random.randI(mhud_empty.size())],TRUE);
	else 
	{
		if(IsZoomed())
		{
			m_pHUD->animPlay(mhud_idle_aim[Random.randI(mhud_idle_aim.size())], TRUE);
		}
		else
		{
			m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())],TRUE);
		}
	}
}

void CWeaponPistol::PlayAnimReload	()
{	
	if(m_opened) 
		m_pHUD->animPlay(mhud_reload_empty[Random.randI(mhud_reload_empty.size())], TRUE, this);
	else
		m_pHUD->animPlay(mhud_reload[Random.randI(mhud_reload.size())], TRUE, this);
	
	m_opened = false;		
}


void CWeaponPistol::PlayAnimHide()
{
	if(m_opened) 
	{
		UpdateFP	();
		PlaySound	(sndClose,vLastFP);
		m_pHUD->animPlay (mhud_close[Random.randI(mhud_close.size())],TRUE,this);
	} 
	else 
		inherited::PlayAnimHide();
}

void CWeaponPistol::PlayAnimShoot	()
{
	if(iAmmoElapsed > 1) 
	{
		m_pHUD->animPlay	(mhud_shots[Random.randI(mhud_shots.size())],FALSE,this);
		m_opened = false;
	}
	else 
	{
		m_pHUD->animPlay	(mhud_shot_l[Random.randI(mhud_shot_l.size())],FALSE,this); 
		m_opened = true; 
	}
}


void CWeaponPistol::switch2_Fire()
{
	inherited::switch2_Fire();
	///m_bPending = false;
}


void CWeaponPistol::switch2_Reload()
{
	if(STATE==eReload) return;
	inherited::switch2_Reload();
}

void CWeaponPistol::OnAnimationEnd()
{
	if(STATE == eHiding && m_opened) 
	{
		m_opened = false;
		switch2_Hiding();
	} else inherited::OnAnimationEnd();
}

void CWeaponPistol::OnShot		()
{
	// Sound
	UpdateFP		();
	PlaySound		(*m_pSndShotCurrent,vLastFP);

	AddShotEffector	();
	
	PlayAnimShoot	();

	// Shell Drop
	Fvector vel; 
	PHGetLinearVell(vel);
	OnShellDrop					(vLastSP,  vel);

	// ќгонь из ствола
	
	StartFlameParticles	();
	R_ASSERT2(!m_pFlameParticles || !m_pFlameParticles->IsLooped(),
			  "can't set looped particles system for shoting with pistol");
	
	//дым из ствола
	StartSmokeParticles	(vLastFP, vel);
}

void CWeaponPistol::UpdateSounds()
{
	inherited::UpdateSounds();

	UpdateFP();
	if (true/*sndClose.snd.feedback*/) sndClose.set_position(vLastFP);
}