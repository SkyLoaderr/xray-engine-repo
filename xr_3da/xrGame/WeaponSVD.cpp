#include "stdafx.h"
#include "weaponsvd.h"

CWeaponSVD::CWeaponSVD(void) : CWeaponCustomPistol("SVD")
{
}

CWeaponSVD::~CWeaponSVD(void)
{
}

void CWeaponSVD::switch2_Fire	()
{
	m_bFireSingleShot = true;
	bWorking = false;
	m_bPending = true;
}

void CWeaponSVD::OnAnimationEnd() 
{
	switch(STATE) 
	{
	case eFire:	{
		m_bPending = false;
		}break;	// End of reload animation
	}
	inherited::OnAnimationEnd();
}
