#include "stdafx.h"
#include "WeaponLR300.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponLR300::CWeaponLR300		() : CWeaponMagazined("LR300",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
}

CWeaponLR300::~CWeaponLR300		()
{
}

void	CWeaponLR300::UpdateCL	()
{
	inherited::UpdateCL	();
	Msg					("- %c%c - %3.1f,%3.1f,%3.1f", getVisible()?'+':'-', getEnabled()?'+':'-', VPUSH(Position()) );
}
