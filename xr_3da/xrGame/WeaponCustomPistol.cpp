#include "stdafx.h"

#include "Entity.h"
#include "WeaponCustomPistol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponCustomPistol::CWeaponCustomPistol(LPCSTR name) : CWeaponMagazined(name,SOUND_TYPE_WEAPON_PISTOL)
{
}

CWeaponCustomPistol::~CWeaponCustomPistol()
{
}
void CWeaponCustomPistol::switch2_Fire	()
{
	m_bFireSingleShot = true;
	bWorking = false;
}


void	CWeaponCustomPistol::state_Fire	(float dt) 
{
	inherited::state_Fire(dt);
}
void	CWeaponCustomPistol::state_MagEmpty	(float dt) 
{
	inherited::state_MagEmpty(dt);
}
void CWeaponCustomPistol::OnAnimationEnd() {
	switch(STATE) {
		case eIdle:
		case eMagEmpty:
		return;
	}
	return inherited::OnAnimationEnd();
}

void CWeaponCustomPistol::FireEnd() 
{
	if(fTime<=0) 
	{
		m_bPending = false;
		inherited::FireEnd();
	}
}