#include "stdafx.h"
#include "weaponwalther.h"

CWeaponWalther::CWeaponWalther(void) : CWeaponPistol("WALTHER")
{
	m_weight = .5f;
	m_slot = 1;
}

CWeaponWalther::~CWeaponWalther(void)
{
}
