#include "stdafx.h"
#include "WeaponAK74.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponAK74::CWeaponAK74() : CWeaponMagazined("AK74",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponAK74::~CWeaponAK74()
{
}

