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
#include "WeaponHPSA.h"
#include "entity.h"
#include "xr_weapon_list.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponHPSA::CWeaponHPSA() : CWeaponMagazined("HPSA")
{
}

CWeaponHPSA::~CWeaponHPSA()
{
}
void CWeaponHPSA::switch2_Fire	(BOOL bHUDView)
{
	// Fire
	Fvector						p1, d;
	CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
	if (E) E->g_fireParams		(p1,d);
	bFlame						=	TRUE;
	OnShot						(bHUDView);
	FireTrace					(p1,vLastFP,d);
}
void CWeaponHPSA::switch2_Empty	(BOOL bHUDView)
{
	pSounds->Play3DAtPos(sndEmptyClick,this,vLastFP);
}
void CWeaponHPSA::OnEmptyClick	(BOOL bHUDView)
{
}

void	CWeaponHPSA::state_Fire		(BOOL bHUD, float dt)
{
}
void	CWeaponHPSA::state_MagEmpty	(BOOL bHUD, float dt)
{
}
