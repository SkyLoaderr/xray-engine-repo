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
	m_bFireSingleShot			= true;
	bWorking					= false;
	m_iShotNum					= 0;
	m_bStopedAfterQueueFired	= false;
}


void CWeaponCustomPistol::OnAnimationEnd() {
/*
switch(STATE) {
		case eIdle:
		case eMagEmpty:
		return;
	}
*/
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