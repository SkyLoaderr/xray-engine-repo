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
	m_eSoundShow = SOUND_TYPE_WEAPON_CHANGING_PISTOL;
	m_eSoundHide = SOUND_TYPE_WEAPON_HIDING_PISTOL;
	m_eSoundShot = SOUND_TYPE_WEAPON_SHOOTING_PISTOL;
	m_eSoundEmptyClick = SOUND_TYPE_WEAPON_EMPTY_CLICKING_PISTOL;
	m_eSoundReload = SOUND_TYPE_WEAPON_RECHARGING_PISTOL;
	m_eSoundRicochet = SOUND_TYPE_WEAPON_BULLET_RICOCHET_PISTOL;
}

CWeaponHPSA::~CWeaponHPSA()
{
}
void CWeaponHPSA::switch2_Fire	(BOOL bHUDView)
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
void CWeaponHPSA::switch2_Empty	(BOOL bHUDView)
{
	pSounds->PlayAtPos(sndEmptyClick,this,vLastFP);
}
void CWeaponHPSA::OnEmptyClick	(BOOL bHUDView)
{
}

void	CWeaponHPSA::state_Fire		(BOOL bHUD, float dt)
{
	fTime					-=dt;
}
void	CWeaponHPSA::state_MagEmpty	(BOOL bHUD, float dt)
{
}
