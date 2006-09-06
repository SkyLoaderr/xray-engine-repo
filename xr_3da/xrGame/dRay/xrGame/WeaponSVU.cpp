#include "stdafx.h"
#include "weaponsvu.h"

CWeaponSVU::CWeaponSVU(void) : CWeaponCustomPistol("SVU")
{
}

CWeaponSVU::~CWeaponSVU(void)
{
}

#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CWeaponSVU::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponSVU,CGameObject>("CWeaponSVU")
			.def(constructor<>())
	];
}
