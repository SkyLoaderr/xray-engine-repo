#include "stdafx.h"

#include "Entity.h"
#include "WeaponCustomPistol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponCustomPistol::CWeaponCustomPistol(LPCSTR name) : CWeaponMagazined(name,SOUND_TYPE_WEAPON_PISTOL)
{
}

CWeaponCustomPistol::~CWeaponCustomPistol()
{
}
void CWeaponCustomPistol::switch2_Fire	(BOOL bHUDView)
{
	if (fTime<=0){
		// Fire
		Fvector						p1, d;
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);
		bFlame						=	TRUE;
		OnShot						(bHUDView);
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire;
	}
}
void CWeaponCustomPistol::switch2_Empty	(BOOL bHUDView)
{
	pSounds->PlayAtPos(sndEmptyClick,this,vLastFP);
}
void CWeaponCustomPistol::OnEmptyClick	(BOOL bHUDView)
{
}

void	CWeaponCustomPistol::state_Fire		(BOOL bHUD, float dt)
{
	fTime					-=dt;
}
void	CWeaponCustomPistol::state_MagEmpty	(BOOL bHUD, float dt)
{
}
