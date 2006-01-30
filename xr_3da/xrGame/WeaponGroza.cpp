#include "stdafx.h"
#include "weapongroza.h"
#include "WeaponHUD.h"

CWeaponGroza::CWeaponGroza(void) : CWeaponMagazinedWGrenade("GROZA",SOUND_TYPE_WEAPON_SUBMACHINEGUN) {
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponGroza::~CWeaponGroza(void) 
{
}

#include "script_space.h"

using namespace luabind;

void CWeaponGroza::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponGroza,CGameObject>("CWeaponGroza")
			.def(constructor<>())
	];
}
