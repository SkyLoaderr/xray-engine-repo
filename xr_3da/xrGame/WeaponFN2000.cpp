#include "stdafx.h"
#include "WeaponFN2000.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFN2000::CWeaponFN2000() : CWeaponMagazined("FN2000")
{
	m_eSoundShow = SOUND_TYPE_WEAPON_CHANGING_SNIPERRIFLE;
	m_eSoundHide = SOUND_TYPE_WEAPON_HIDING_SNIPERRIFLE;
	m_eSoundShot = SOUND_TYPE_WEAPON_SHOOTING_SNIPERRIFLE;
	m_eSoundEmptyClick = SOUND_TYPE_WEAPON_EMPTY_CLICKING_SNIPERRIFLE;
	m_eSoundReload = SOUND_TYPE_WEAPON_RECHARGING_SNIPERRIFLE;
	m_eSoundRicochet = SOUND_TYPE_WEAPON_BULLET_RICOCHET_SNIPERRIFLE;
}

CWeaponFN2000::~CWeaponFN2000()
{
}

