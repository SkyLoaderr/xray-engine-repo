#include "stdafx.h"
#include "weaponpistol.h"
#include "WeaponHUD.h"
#include "effectorshot.h"
#include "..\PGObject.h"

CWeaponPistol::CWeaponPistol(LPCSTR name) : CWeaponCustomPistol(name)
{
	m_eSoundClose		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING /*| eSoundType*/);
	m_opened = false;
}

CWeaponPistol::~CWeaponPistol(void)
{
}

void CWeaponPistol::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	SoundDestroy		(sndClose		);
}

void CWeaponPistol::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	m_opened = false;
}

void CWeaponPistol::switch2_Idle	()
{
	if(m_opened) m_pHUD->animPlay(mhud_empty[Random.randI(mhud_empty.size())]);
	else m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}
void CWeaponPistol::switch2_Hiding()
{
	if(m_opened) {
		Sound->play_at_pos		(sndClose,H_Root(),vLastFP);
		if (sndClose.feedback)
			sndClose.feedback->set_volume(.2f);
		m_pHUD->animPlay		(mhud_close[Random.randI(mhud_close.size())],FALSE,this);
	} else inherited::switch2_Hiding	();
}
void CWeaponPistol::switch2_Reload()
{
	m_opened = false;
	inherited::switch2_Reload();
}
void CWeaponPistol::OnAnimationEnd()
{
	if(STATE == eHiding && m_opened) {
		m_opened = false;
		switch2_Hiding();
	} else inherited::OnAnimationEnd();
}

void CWeaponPistol::Load	(LPCSTR section)
{
	inherited::Load		(section);

	SoundCreate			(sndClose,		"close"  ,m_eSoundClose);

	animGet				(mhud_empty,	"empty");
	animGet				(mhud_shot_l,	"shoot_last");
	animGet				(mhud_close,	"close");
}

void CWeaponPistol::OnShot		()
{
	// Sound
	Sound->play_at_pos			(sndShot,H_Root(),vLastFP);

	// Camera
	if (hud_mode)	
	{
		CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
		if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot>(camMaxAngle,camRelaxSpeed));
		R_ASSERT				(S);
		S->Shot					(camDispersion);
	}
	
	// Animation
	if(/*m_pAmmo->m_magCurr*/iAmmoElapsed > 1) m_pHUD->animPlay			(mhud_shots[Random.randI(mhud_shots.size())],FALSE,this);
	else { m_pHUD->animPlay			(mhud_shot_l[Random.randI(mhud_shot_l.size())],FALSE,this); m_opened = true; }
	
	// Flames
	fFlameTime					= .1f;
	
	// Shell Drop
	OnShellDrop					();

	CPGObject* pStaticPG;/* s32 l_c = m_effects.size();*/
	pStaticPG = xr_new<CPGObject>("weapons\\generic_shoot",Sector());
	Fmatrix l_pos; l_pos.set(svTransform); l_pos.c.set(vLastFP);
	Fvector l_vel; l_vel.sub(vPosition,ps_Element(0).vPosition); l_vel.div((Device.dwTimeGlobal-ps_Element(0).dwTime)/1000.f);
	pStaticPG->UpdateParent(l_pos, l_vel); pStaticPG->Play();
	//pStaticPG->SetTransform(l_pos); pStaticPG->Play();
}
