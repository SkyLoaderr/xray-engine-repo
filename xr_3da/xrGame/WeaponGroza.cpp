#include "stdafx.h"
#include "weapongroza.h"
#include "WeaponHUD.h"

CWeaponGroza::CWeaponGroza(void) : CWeaponMagazinedWGrenade("GROZA",SOUND_TYPE_WEAPON_SUBMACHINEGUN) {
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponGroza::~CWeaponGroza(void) {
}
//
//void CWeaponGroza::Load	(LPCSTR section)
//{
//	inherited::Load		(section);
//	//// Sounds
//	//SoundCreate			(sndShow,		"draw"    ,m_eSoundShow);
//	//SoundCreate			(sndHide,		"holster" ,m_eSoundHide);
//	//SoundCreate			(sndShot,		"shoot"   ,m_eSoundShot);
//	//SoundCreate			(sndEmptyClick,	"empty"   ,m_eSoundEmptyClick);
//	//SoundCreate			(sndReload,		"reload"  ,m_eSoundReload);
//	//SoundCreate			(sndRicochet[0],"ric1"    ,m_eSoundRicochet);
//	//SoundCreate			(sndRicochet[1],"ric2"    ,m_eSoundRicochet);
//	//SoundCreate			(sndRicochet[2],"ric3"    ,m_eSoundRicochet);
//	//SoundCreate			(sndRicochet[3],"ric4"    ,m_eSoundRicochet);
//	//SoundCreate			(sndRicochet[4],"ric5"    ,m_eSoundRicochet);
//	// HUD :: Anims
//	R_ASSERT			(m_pHUD);
//	animGet				(mhud_idle_g,		"idle_g");
//	animGet				(mhud_reload_g,	"reload_g");
//	animGet				(mhud_shots_g,	"shoot_g");
//	animGet				(mhud_switch_g,	"switch_g");
//	animGet				(mhud_switch,	"switch");
//}
//void CWeaponGroza::switch2_Idle() {
//	if(m_grenadeMode) m_pHUD->animPlay(mhud_idle_g[Random.randI(mhud_idle_g.size())]);
//	else inherited::switch2_Idle();
//}
//void CWeaponGroza::switch2_Reload()
//{
//	//Sound->play_at_pos		(sndReload,H_Root(),vLastFP);
//	//if (sndReload.feedback)
//	//	sndReload.feedback->set_volume(.2f);
//	if(m_grenadeMode) m_pHUD->animPlay(mhud_reload_g[Random.randI(mhud_reload_g.size())],FALSE,this);
//	else inherited::switch2_Reload();
//}
//
//void CWeaponGroza::OnShot		()
//{
//	if(m_grenadeMode) {
//		m_pHUD->animPlay(mhud_shots_g[Random.randI(mhud_shots_g.size())],TRUE,this);
//	} else inherited::OnShot();
//	//// Sound
//	//Sound->play_at_pos			(sndShot,H_Root(),vLastFP);
//
//	//// Camera
//	//if (hud_mode)	
//	//{
//	//	CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
//	//	if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot> (camMaxAngle,camRelaxSpeed));
//	//	R_ASSERT				(S);
//	//	S->Shot					(camDispersion);
//	//}
//	//
//	//// Animation
//	//m_pHUD->animPlay			(mhud_shots[Random.randI(mhud_shots.size())],TRUE,this);
//	//
//	//// Flames
//	//fFlameTime					= .1f;
//	//
//	//// Shell Drop
//	//OnShellDrop					();
//
//	//CPGObject* pStaticPG;/* s32 l_c = m_effects.size();*/
//	//pStaticPG = xr_new<CPGObject>("weapons\\generic_shoot",Sector());
//	//Fmatrix l_pos; l_pos.set(XFORM()); l_pos.c.set(vLastFP);
//	//Fvector l_vel; l_vel.sub(Position(),ps_Element(0).Position()); l_vel.div((Device.dwTimeGlobal-ps_Element(0).dwTime)/1000.f);
//	//pStaticPG->UpdateParent(l_pos, l_vel); pStaticPG->Play();
//	////pStaticPG->SetTransform(l_pos); pStaticPG->Play();
//}
//
//void CWeaponGroza::SwitchMode() {
//	m_grenadeMode = !m_grenadeMode;
//	if(m_grenadeMode) m_pHUD->animPlay(mhud_switch_g[Random.randI(mhud_switch_g.size())],FALSE,this);
//	else m_pHUD->animPlay(mhud_switch[Random.randI(mhud_switch.size())],FALSE,this);
//}
//
//bool CWeaponGroza::Action(s32 cmd, u32 flags) {
//	switch(cmd) {
//		case kWPN_NEXT : {
//			if(flags&CMD_START) SwitchMode();
//		} return true;
//	}
//	if(inherited::Action(cmd, flags)) return true;
//	return false;
//}

