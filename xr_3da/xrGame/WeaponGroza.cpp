#include "stdafx.h"
#include "weapongroza.h"

CWeaponGroza::CWeaponGroza(void) : CWeaponMagazined("GROZA",SOUND_TYPE_WEAPON_SUBMACHINEGUN) {
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponGroza::~CWeaponGroza(void) {
}
