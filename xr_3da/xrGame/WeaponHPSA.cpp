#include "stdafx.h"
#include "WeaponHPSA.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponHPSA::CWeaponHPSA() : CWeaponAutoRifle("HPSA")
{
}

CWeaponHPSA::~CWeaponHPSA()
{
}

void CWeaponHPSA::OnShot(BOOL bHUDView)
{
	inherited::OnShot	(bHUDView);
//	FireEnd				();
}
