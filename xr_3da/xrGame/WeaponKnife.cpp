#include "stdafx.h"

#include "WeaponKnife.h"
#include "WeaponHUD.h"
#include "Entity.h"
#include "Actor.h"
#include "level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"


#define KNIFE_MATERIAL_NAME "objects\\bullet"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponKnife::CWeaponKnife() : CWeapon("KNIFE") 
{
	m_attackStart = false;
	m_bShotLight = false;
	STATE = NEXT_STATE = eHidden;

	knife_material_idx = (u16)-1;
}

CWeaponKnife::~CWeaponKnife()
{
}

void CWeaponKnife::Load	(LPCSTR section)
{
	// verify class
	inherited::Load		(section);

	fWallmarkSize = pSettings->r_float(section,"wm_size");

	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		pSettings->r_string(*hud_sect,"anim_idle"));
	animGet				(mhud_hide,		pSettings->r_string(*hud_sect,"anim_hide"));
	animGet				(mhud_show,		pSettings->r_string(*hud_sect,"anim_draw"));
	animGet				(mhud_attack,	pSettings->r_string(*hud_sect,"anim_shoot1_start"));
	animGet				(mhud_attack2,	pSettings->r_string(*hud_sect,"anim_shoot2_start"));
	animGet				(mhud_attack_e,	pSettings->r_string(*hud_sect,"anim_shoot1_end"));
	animGet				(mhud_attack2_e,pSettings->r_string(*hud_sect,"anim_shoot2_end"));

	
	knife_material_idx =  GMLib.GetMaterialIdx(KNIFE_MATERIAL_NAME);
}

void CWeaponKnife::renderable_Render()
{
	UpdateXForm						();

	CActor *pActor = smart_cast<CActor*>(H_Parent());
	if (pActor && pActor->HUDview() && m_pHUD && !m_pHUD->IsHidden()) { 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else
		inherited::renderable_Render();
}

void CWeaponKnife::OnStateSwitch	(u32 S)
{
	switch (S)
	{
	case eIdle:
		switch2_Idle	();
		break;
	case eShowing:
		switch2_Showing	();
		break;
	case eHiding:
		switch2_Hiding	();
		break;
	case eHidden:
		switch2_Hidden	();
		break;
	case eFire:
		switch2_Attacking	();
		break;
	case eFire2:
		switch2_Attacking2 ();
		break;
	}
	STATE = S;
	NEXT_STATE = S;
}
	
void CWeaponKnife::UpdateCL	()
{
	inherited::UpdateCL	();

	
	// cycle update
	switch (STATE)
	{
	case eShowing:
	case eHiding:
		smart_cast<CSkeletonAnimated*>(m_pHUD->Visual())->Update();
		break;
	case eFire:
		//state_Attacking	(dt);
		break;
	}

	// setVisible			(TRUE);

	// ref_sound fire loop
	///UpdateFP					();
}

void CWeaponKnife::Hide	()
{
	//if (Local())	SwitchState						(eHiding);
	SwitchState(eHidden);
}

void CWeaponKnife::Show	()
{
	if (Local())				
	{
		SwitchState						(eShowing);
	}
}

void CWeaponKnife::KnifeStrike(const Fvector& pos, const Fvector& dir)
{
	CCartridge cartridge; 
	cartridge.m_buckShot = 1;				
	cartridge.m_impair = 1;
	cartridge.m_kDisp = 1;
	cartridge.m_kHit = 1;
	cartridge.m_kImpulse = 1;
	cartridge.m_kPierce = 1;
	cartridge.m_tracer = false;
	cartridge.m_ricochet = false;
	cartridge.fWallmarkSize = fWallmarkSize;
	cartridge.bullet_material_idx = knife_material_idx;

	while(m_magazine.size() < 2) m_magazine.push(cartridge);
	iAmmoElapsed = m_magazine.size();
	FireTrace(pos,dir);
}


void CWeaponKnife::StartIdleAnim		()
{
	m_pHUD->animDisplay(mhud_attack_e[Random.randI(mhud_attack_e.size())], TRUE);
}
void CWeaponKnife::OnAnimationEnd()
{
	switch (STATE)
	{
	case eHiding:	SwitchState(eHidden);	break;	// End of Hide
	case eFire: 
		{
            if(m_attackStart) 
			{
				m_attackStart = false;
				m_pHUD->animPlay(mhud_attack_e[Random.randI(mhud_attack_e.size())], TRUE, this);
				Fvector	p1, d; 
				p1.set(vLastFP); 
				d.set(vLastFD);

				if(H_Parent()) 
					smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1,d);
				else break;

				KnifeStrike(p1,d);
			} 
			else 
				SwitchState(eIdle);
		}break;
	case eFire2: 
		{
            if(m_attackStart) 
			{
				m_attackStart = false;
				m_pHUD->animPlay(mhud_attack2_e[Random.randI(mhud_attack2_e.size())], TRUE, this);
				
				Fvector	p1, d; 
				p1.set(vLastFP); 
				d.set(vLastFD);
				
				if(H_Parent()) 
					smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1,d);
				else break;
			
				KnifeStrike(p1,d);
			} else 
				SwitchState(eIdle);
		} break;
	case eShowing:									// End of Show
	case eIdle:	
		SwitchState(eIdle);		break;	
	}
}

void CWeaponKnife::state_Attacking	(float /**dt/**/)
{
}

void CWeaponKnife::switch2_Attacking	()
{
	if(m_bPending) return;

	m_pHUD->animPlay(mhud_attack[Random.randI(mhud_attack.size())],		FALSE, this);
	m_attackStart = true;
	m_bPending = true;
}

void CWeaponKnife::switch2_Attacking2	()
{
	if(m_bPending) return;

	m_pHUD->animPlay(mhud_attack2[Random.randI(mhud_attack2.size())],	FALSE, this);
	m_attackStart = true;
	m_bPending = true;
}


void CWeaponKnife::switch2_Idle	()
{
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
	m_bPending = false;
}

void CWeaponKnife::switch2_Hiding	()
{
	FireEnd					();
	
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
	if (Local())			Level().Cameras.RemoveEffector	(cefShot);

	m_bPending				= true;
}

void CWeaponKnife::switch2_Hidden()
{
	signal_HideComplete		();
}

void CWeaponKnife::switch2_Showing	()
{
	setVisible				(TRUE);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);

	m_bPending				= true;
}


void CWeaponKnife::FireStart()
{
	inherited::FireStart();
	SwitchState(eFire);
}

void CWeaponKnife::FireEnd()
{
	inherited::FireEnd();
}


void CWeaponKnife::Fire2Start () 
{
	inherited::Fire2Start();
	SwitchState(eFire2);
}

void CWeaponKnife::Fire2End () 
{
	inherited::Fire2End();
}

const char* CWeaponKnife::Name() 
{
	return CInventoryItem::Name();
}

bool CWeaponKnife::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) 
	{
		case kWPN_ZOOM : 
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
			return true;
		case kDROP:
			{
				if (GameID() != GAME_SINGLE)
					return true;
			}break;
	}
	return false;
}