#include "stdafx.h"
#include "weaponpistol.h"
#include "WeaponHUD.h"
#include "effectorshot.h"
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
	sndClose.destroy();
}

void CWeaponPistol::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	m_opened = false;
}

void CWeaponPistol::PlayAnimIdle	()
{
	if(m_opened) 
		m_pHUD->animPlay(mhud_empty[Random.randI(mhud_empty.size())]);
	else 
		m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}

void CWeaponPistol::PlayAnimHide()
{
	if(m_opened) 
	{
		UpdateFP();
		sndClose.play_at_pos(H_Root(),vLastFP,hud_mode?sm_2D:0);
		///if (sndClose.feedback)	sndClose.feedback->set_volume(.2f);
		m_pHUD->animPlay		(mhud_close[Random.randI(mhud_close.size())],FALSE,this);
	} 
	else 
		inherited::PlayAnimHide();
}
void CWeaponPistol::switch2_Reload()
{
	m_opened = false;
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

void CWeaponPistol::Load	(LPCSTR section)
{
	inherited::Load		(section);

	sndClose.create		(TRUE, pSettings->r_string(section, "snd_close"), m_eSoundClose);

	animGet				(mhud_empty,	pSettings->r_string(*hud_sect, "anim_empty"));
	animGet				(mhud_shot_l,	pSettings->r_string(*hud_sect, "anim_shot_last"));
	animGet				(mhud_close,	pSettings->r_string(*hud_sect, "anim_close"));
}

void CWeaponPistol::OnShot		()
{
	// Sound
	UpdateFP();
	sndShot.play_at_pos(H_Root(),vLastFP,hud_mode?sm_2D:0, sndShot_delay);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot>(camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// Animation
	if(iAmmoElapsed > 1) 
		m_pHUD->animPlay	(mhud_shots[Random.randI(mhud_shots.size())],FALSE,this);
	else 
	{
		m_pHUD->animPlay			(mhud_shot_l[Random.randI(mhud_shot_l.size())],FALSE,this); 
		m_opened = true; 
	}

	// Shell Drop
	OnShellDrop					();

	// ����� �� ������
	StartFlameParticles	();

	//��� �� ������
	StartSmokeParticles	();
}
