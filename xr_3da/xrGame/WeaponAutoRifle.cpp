#include "stdafx.h"
#include "effectorshot.h"
#include "..\render.h"
#include "..\bodyinstance.h"
#include "..\xr_tokens.h"
#include "..\3DSound.h"
#include "..\PSObject.h"
#include "..\xr_trims.h"
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

