#include "stdafx.h"
#include "WeaponAK74.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponAK74::CWeaponAK74() : CWeaponMagazined("AK74")
{
	m_eSoundShow = SOUND_TYPE_WEAPON_CHANGING_SUBMACHINEGUN;
	m_eSoundHide = SOUND_TYPE_WEAPON_HIDING_SUBMACHINEGUN;
	m_eSoundShot = SOUND_TYPE_WEAPON_SHOOTING_SUBMACHINEGUN;
	m_eSoundEmptyClick = SOUND_TYPE_WEAPON_EMPTY_CLICKING_SUBMACHINEGUN;
	m_eSoundReload = SOUND_TYPE_WEAPON_RECHARGING_SUBMACHINEGUN;
	m_eSoundRicochet = SOUND_TYPE_WEAPON_BULLET_RICOCHET_SUBMACHINEGUN;
}

CWeaponAK74::~CWeaponAK74()
{
}

