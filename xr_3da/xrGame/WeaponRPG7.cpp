#include "stdafx.h"

#include "weaponrpg7.h"

#include "WeaponHUD.h"
#include "xrserver_objects_alife_items.h"

#include "explosiverocket.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////
CWeaponRPG7::CWeaponRPG7(void) : CWeaponCustomPistol("RPG7") 
{
	m_weight = 5.f;
	m_slot = 2;
	m_hideGrenade = false;
	m_pGrenadePoint = &vLastFP;
}

CWeaponRPG7::~CWeaponRPG7(void) 
{
}

void CWeaponRPG7::Load	(LPCSTR section)
{
	inherited::Load			(section);
	CRocketLauncher::Load	(section);

	m_fScopeZoomFactor		= pSettings->r_float	(section,"max_zoom_factor");

	m_sGrenadeBoneName		= pSettings->r_string	(section,"grenade_bone");
	m_sHudGrenadeBoneName	= pSettings->r_string	(hud_sect,"grenade_bone");

	m_sRocketSection		= pSettings->r_string	(section,"rocket_class");
}

BOOL CWeaponRPG7::net_Spawn(LPVOID DC) 
{
	m_pGrenadePoint = &vLastFP;

	BOOL l_res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(m_pHUD->Visual()); R_ASSERT(V);
	V->LL_GetBoneInstance(V->LL_BoneID(*m_sHudGrenadeBoneName)).set_callback(GrenadeCallback, this);
	
	V = PKinematics(Visual()); R_ASSERT(V);
	V->LL_GetBoneInstance(V->LL_BoneID(*m_sGrenadeBoneName)).set_callback(GrenadeCallback, this);

	m_hideGrenade = !iAmmoElapsed;
	if(iAmmoElapsed && !m_pRocket)
	{
		CRocketLauncher::SpawnRocket(*m_sRocketSection, dynamic_cast<CGameObject*>(H_Parent()));
	}

	return l_res;
}

void __stdcall CWeaponRPG7::GrenadeCallback(CBoneInstance* B) 
{
	CWeaponRPG7* l_pW = dynamic_cast<CWeaponRPG7*>(static_cast<CObject*>(B->Callback_Param)); R_ASSERT(l_pW);
	if(l_pW->m_hideGrenade) B->mTransform.scale(EPS, EPS, EPS);
}

void CWeaponRPG7::OnStateSwitch(u32 S) 
{
	inherited::OnStateSwitch(S);
	m_hideGrenade = (!iAmmoElapsed && !(S == eReload));
}

void CWeaponRPG7::ReloadMagazine() 
{
	inherited::ReloadMagazine();

	if(iAmmoElapsed && !m_pRocket) 
	{
		CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
	}
}
void CWeaponRPG7::SwitchState(u32 S) 
{
	inherited::SwitchState(S);
	if(STATE == eIdle && S==eFire && m_pRocket) 
	{
		Fvector p1, d; 
		p1.set(vLastFP); 
		d.set(vLastFD);

		CEntity* E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams (this, p1,d);
		
		Fmatrix launch_matrix;
		launch_matrix.identity();
		launch_matrix.k.set(d);
		Fvector::generate_orthonormal_basis(launch_matrix.k,
											launch_matrix.i, launch_matrix.j);
		launch_matrix.c.set(p1);

		d.normalize();
		d.mul(m_fLaunchSpeed);
		
		Fvector angular_vel;
		angular_vel.set(d);
		angular_vel.mul(1400.f);
		CRocketLauncher::LaunchRocket(launch_matrix, d, angular_vel);

		CExplosiveRocket* pGrenade = dynamic_cast<CExplosiveRocket*>(m_pRocket);
		VERIFY(pGrenade);
		pGrenade->SetCurrentParentID(H_Parent()->ID());

		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16(m_pRocket->ID()));
		u_EventSend(P);
	}
}

void CWeaponRPG7::FireStart()
{
	inherited::FireStart();
}

void CWeaponRPG7::switch2_Fire	()
{
}

void CWeaponRPG7::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CRocketLauncher::AttachRocket(id, this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			CRocketLauncher::DetachRocket(id);
		} break;
	}
}