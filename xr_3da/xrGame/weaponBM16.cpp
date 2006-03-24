#include "stdafx.h"
#include "weaponBM16.h"
#include "WeaponHUD.h"

CWeaponBM16::~CWeaponBM16()
{
	HUD_SOUND::DestroySound(m_sndReload1);
}

void CWeaponBM16::Load	(LPCSTR section)
{
	inherited::Load		(section);

	animGet	(mhud_reload1,	pSettings->r_string(*hud_sect,"anim_reload_1"));
	animGet	(mhud_shot1,	pSettings->r_string(*hud_sect,"anim_shoot_1"));
	animGet	(mhud_idle1,		pSettings->r_string(*hud_sect,"anim_idle_1"));
	animGet	(mhud_idle2,		pSettings->r_string(*hud_sect,"anim_idle_2"));
	animGet	(mhud_zoomed_idle1,		pSettings->r_string(*hud_sect,"anim_zoomed_idle_1"));
	animGet	(mhud_zoomed_idle2,		pSettings->r_string(*hud_sect,"anim_zoomedidle_2"));


	HUD_SOUND::LoadSound(section, "snd_reload_1", m_sndReload1, TRUE, m_eSoundShot);

}

void CWeaponBM16::PlayReloadSound()
{
	if(m_magazine.size()==1)	PlaySound	(m_sndReload1,get_LastFP());
	else						PlaySound	(sndReload,get_LastFP());
}

void CWeaponBM16::PlayAnimShoot()
{
	if(m_magazine.size()==1)
		m_pHUD->animPlay(mhud_shot1[Random.randI(mhud_shot1.size())],TRUE,this);
	else
		m_pHUD->animPlay(mhud.mhud_shots[Random.randI(mhud.mhud_shots.size())],TRUE,this);
}

void CWeaponBM16::PlayAnimReload()
{
	bool b_both = HaveCartridgeInInventory(2);

	if(m_magazine.size()==1 || !b_both)
		m_pHUD->animPlay(mhud_reload1[Random.randI(mhud_reload1.size())],TRUE,this);
	else
		m_pHUD->animPlay(mhud.mhud_reload[Random.randI(mhud.mhud_reload.size())],TRUE,this);

}

void CWeaponBM16::PlayAnimIdle()
{
	if(TryPlayAnimIdle())	return;

	if(IsZoomed())
	{
		switch (m_magazine.size())
		{
		case 0:{
			m_pHUD->animPlay(mhud.mhud_idle_aim[Random.randI(mhud.mhud_idle_aim.size())], TRUE);
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
			m_pHUD->animPlay(mhud.mhud_idle[Random.randI(mhud.mhud_idle.size())], TRUE);
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

#include "script_space.h"

using namespace luabind;

void CWeaponBM16::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponBM16,CGameObject>("CWeaponBM16")
			.def(constructor<>())
	];
}
