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
void CWeaponCustomPistol::switch2_Fire	()
{
	if (fTime<=0)
	{
		bPending = true;
		UpdateFP					();

		// Fire
		Fvector						p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) {
			E->g_fireParams		(p1,d);
			bFlame						=	TRUE;
			OnShot						();
			FireTrace					(p1,vLastFP,d);
		}
		fTime						+= fTimeToFire;

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

/*
void CWeaponCustomPistol::switch2_Empty	()
{
	Sound->play_at_pos(sndEmptyClick,this,vLastFP);
}

void CWeaponCustomPistol::OnEmptyClick	()
{
}
*/

void	CWeaponCustomPistol::state_Fire	(float dt) 
{
	fTime -=dt;
	//if(fTime<=0) inherited::FireEnd();
}
void	CWeaponCustomPistol::state_MagEmpty	(float /**dt/**/) 
{
	inherited::FireEnd();
}
void CWeaponCustomPistol::OnAnimationEnd() {
	switch(STATE) {
		case eIdle:
		case eMagEmpty:
		case eFire: {
			inherited::FireEnd();
		} return;
	}
	return inherited::OnAnimationEnd();
}

void CWeaponCustomPistol::FireEnd() {
	if(fTime<=0) 
	{
		bPending = false;
		inherited::FireEnd();
	}
}
