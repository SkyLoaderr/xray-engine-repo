#include "stdafx.h"
#include "effectorshot.h"
#include "..\PSObject.h"
#include "hudmanager.h"

#include "WeaponHUD.h"
#include "WeaponAutoRifle.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponAutoRifleA::CWeaponAutoRifleA(LPCSTR name, ESoundTypes eSoundType) : CWeaponMagazined(name, eSoundType)
{
}

CWeaponAutoRifleA::~CWeaponAutoRifleA()
{
}

