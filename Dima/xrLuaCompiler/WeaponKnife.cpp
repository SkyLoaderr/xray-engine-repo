#include "stdafx.h"

#include "WeaponKnife.h"
#include "WeaponHUD.h"
#include "xr_weapon_list.h"
#include "Entity.h"
#include "Actor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponKnife::CWeaponKnife() : CWeapon("KNIFE") {
	m_attackStart = false;
	m_shotLight = false;

}

CWeaponKnife::~CWeaponKnife()
{
}

void CWeaponKnife::Load	(LPCSTR section)
{
	// verify class
	inherited::Load		(section);
	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		"idle");
	animGet				(mhud_show,		"draw");
	animGet				(mhud_hide,		"hide");
	animGet				(mhud_attack,	"shoot1_start");	
	animGet				(mhud_attack2,	"shoot2_start");	
	animGet				(mhud_attack_e,	"shoot1_end");	
	animGet				(mhud_attack2_e,	"shoot2_end");	
}

void CWeaponKnife::renderable_Render()
{
	inherited::renderable_Render	();
	UpdateXForm						();
	CActor *l_pA = dynamic_cast<CActor*>(H_Parent());
	if (l_pA && l_pA->HUDview() && m_pHUD)
	{ 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else if(!l_pA || !l_pA->HUDview())
	{
		// Actor render
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
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

//	float dt			= Device.fTimeDelta;
	
	// cycle update
	switch (STATE)
	{
	case eShowing:
	case eHiding:
		PKinematics		(m_pHUD->Visual())->Update();
		break;
	case eFire:
		//state_Attacking	(dt);
		break;
	}

	// setVisible			(TRUE);
	bPending			= FALSE;
	
	// ref_sound fire loop
	UpdateFP					();
}

void CWeaponKnife::Hide	()
{
	if (Local())	SwitchState						(eHiding);
}

void CWeaponKnife::Show	()
{
	if (Local())				
	{
		SwitchState						(eShowing);
	}
}

void CWeaponKnife::OnAnimationEnd()
{
	switch (STATE)
	{
	case eHiding:	SwitchState(eHidden);	break;	// End of Hide
	case eFire: {
		if(m_attackStart) {
			m_attackStart = false;
			m_pHUD->animPlay(mhud_attack_e[Random.randI(mhud_attack_e.size())], FALSE, this);
			Fvector					p1, d; p1.set(vLastFP); d.set(vLastFD);
			if(H_Parent()) dynamic_cast<CEntity*>(H_Parent())->g_fireParams(p1,d);
			else break;
			CCartridge l_cartridge; l_cartridge.Load(m_ammoTypes[m_ammoType]);
			while(m_magazine.size() < 2) m_magazine.push(l_cartridge);
			FireTrace(p1,vLastFP,d);
		} else SwitchState(eIdle);
	} break;
	case eFire2: {
		if(m_attackStart) {
			m_attackStart = false;
			m_pHUD->animPlay(mhud_attack2_e[Random.randI(mhud_attack2_e.size())], FALSE, this);
			Fvector					p1, d; p1.set(vLastFP); d.set(vLastFD);
			if(H_Parent()) dynamic_cast<CEntity*>(H_Parent())->g_fireParams(p1,d);
			else break;
			CCartridge l_cartridge; l_cartridge.Load(m_ammoTypes[m_ammoType]);
			while(m_magazine.size() < 2) m_magazine.push(l_cartridge);
			FireTrace(p1,vLastFP,d);
		} else SwitchState(eIdle);
	} break;
	case eShowing:									// End of Show
	case eIdle:	
		SwitchState(eIdle);		break;	
	}
}

void CWeaponKnife::state_Attacking	(float dt)
{
}

void CWeaponKnife::switch2_Attacking	()
{
	m_pHUD->animPlay(mhud_attack[Random.randI(mhud_attack.size())], FALSE, this);
	m_attackStart = true;
}

void CWeaponKnife::switch2_Attacking2	()
{
	m_pHUD->animPlay(mhud_attack2[Random.randI(mhud_attack2.size())], FALSE, this);
	m_attackStart = true;
}


void CWeaponKnife::switch2_Idle	()
{
	//switch (STATE)
	//{
	//case eFire: 
	//	break;
	//}
	m_pHUD->animPlay(mhud_idle[Random.randI(mhud_idle.size())]);
}

void CWeaponKnife::switch2_Hiding	()
{
	FireEnd					();
	bPending				= TRUE;
	m_pHUD->animPlay		(mhud_hide[Random.randI(mhud_hide.size())],TRUE,this);
	if (Local())			Level().Cameras.RemoveEffector	(cefShot);


}
void CWeaponKnife::switch2_Hidden()
{
	signal_HideComplete		();
}

void CWeaponKnife::switch2_Showing	()
{
	setVisible				(TRUE);
	m_pHUD->animPlay		(mhud_show[Random.randI(mhud_show.size())],FALSE,this);
}


void CWeaponKnife::FireStart()
{
	inherited::FireStart();
	OnStateSwitch(eFire);
}

void CWeaponKnife::FireEnd()
{
	inherited::FireEnd();
}


void CWeaponKnife::Fire2Start () {
	inherited::Fire2Start();
	OnStateSwitch(eFire2);
}

void CWeaponKnife::Fire2End () {
	inherited::Fire2End();
}

const char* CWeaponKnife::Name() {
	return CInventoryItem::Name();
}

bool CWeaponKnife::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_ZOOM : 
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
			return true;
	}
	return false;
}
