#include "stdafx.h"

#include "WeaponKnife.h"
#include "WeaponHUD.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponKnife::CWeaponKnife() : CWeapon("KNIFE")
{
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
	animGet				(mhud_idle,		"idle_0");
	animGet				(mhud_show,		"draw");
	animGet				(mhud_hide,		"idle_0");
	animGet				(mhud_attack,	"draw");
}

void CWeaponKnife::OnVisible	()
{
	inherited::OnVisible			();
	UpdateXForm						();
	if (hud_mode && m_pHUD)
	{ 
		// HUD render
		::Render->set_Transform		(&m_pHUD->Transform());
		::Render->add_Visual		(m_pHUD->Visual());
	}
	else
	{
		// Actor render
		::Render->set_Transform		(&svTransform);
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
	}
	STATE = S;
}
	
void CWeaponKnife::UpdateCL	()
{
	inherited::UpdateCL	();

	float dt			= Device.fTimeDelta;
	
	// cycle update
	switch (STATE)
	{
	case eShowing:
	case eHiding:
		PKinematics		(m_pHUD->Visual())->Update();
	case eFire:
		state_Attacking	(dt);
		break;
	}

	// setVisible			(TRUE);
	bPending			= FALSE;
	
	// sound fire loop
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
	case eShowing:	
	case eIdle:	
		SwitchState(eIdle);		break;	// End of Show
	}
}

void CWeaponKnife::state_Attacking	(float dt)
{

}

void CWeaponKnife::switch2_Attacking	()
{

	
}

void CWeaponKnife::switch2_Idle	()
{
	switch (STATE)
	{
	case eFire: 
		break;
	}
	m_pHUD->animPlay(mhud_attack[Random.randI(mhud_idle.size())]);
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



void CWeaponKnife::Fire2Start () {
	inherited::Fire2Start();
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
