#include "stdafx.h"
#include "WeaponAK74.h"
#include "WeaponHUD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponAK74::CWeaponAK74(LPCSTR name, ESoundTypes eSoundType) : CWeaponMagazinedWGrenade(name, eSoundType)
{
//	m_weight = 1.5f; CInventoryItem load this
//	m_slot = 2;
}

CWeaponAK74::~CWeaponAK74()
{
}