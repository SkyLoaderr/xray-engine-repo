#include "stdafx.h"
#include "weaponrpg7.h"
#include "WeaponHUD.h"

CWeaponRPG7::CWeaponRPG7(void) : CWeaponCustomPistol("RPG7") {
	m_weight = 5.f;
	m_slot = 2;
	m_hideGrenade = false;
}

CWeaponRPG7::~CWeaponRPG7(void) {
}

BOOL CWeaponRPG7::net_Spawn(LPVOID DC) {
	BOOL l_res = inherited::net_Spawn(DC);
	CKinematics* V = PKinematics(m_pHUD->Visual()); R_ASSERT(V);
	V->LL_GetInstance(V->LL_BoneID("grenade_0")).set_callback(GrenadeCallback, this);
	V = PKinematics(Visual()); R_ASSERT(V);
	V->LL_GetInstance(V->LL_BoneID("grenade")).set_callback(GrenadeCallback, this);
	m_hideGrenade = !iAmmoElapsed;
	return l_res;
}

void __stdcall CWeaponRPG7::GrenadeCallback(CBoneInstance* B) {
	CWeaponRPG7* l_pW = dynamic_cast<CWeaponRPG7*>(static_cast<CObject*>(B->Callback_Param)); R_ASSERT(l_pW);
	if(l_pW->m_hideGrenade) B->mTransform.scale(0, 0, 0);
}

void CWeaponRPG7::OnStateSwitch(u32 S) {
	inherited::OnStateSwitch(S);
	m_hideGrenade = (!iAmmoElapsed && !(S == eReload));
}