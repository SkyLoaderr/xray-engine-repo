#include "stdafx.h"
#include "WeaponRPG7Grenade.h"
#include "weaponrpg7.h"
#include "WeaponHUD.h"
#include "xrserver_objects_alife_items.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////
CWeaponRPG7::CWeaponRPG7(void) : CWeaponCustomPistol("RPG7") 
{
	m_weight = 5.f;
	m_slot = 2;
	m_hideGrenade = false;
	m_pGrenade = NULL;
	m_pGrenadePoint = &vLastFP;
}

CWeaponRPG7::~CWeaponRPG7(void) 
{
}

void CWeaponRPG7::Load	(LPCSTR section)
{
	inherited::Load			(section);
	m_fScopeZoomFactor		= pSettings->r_float	(section,"max_zoom_factor");

	m_sGrenadeBoneName		= pSettings->r_string	(section,"grenade_bone");
	m_sHudGrenadeBoneName	= pSettings->r_string	(hud_sect,"grenade_bone");
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
	if(iAmmoElapsed && !m_pGrenade) 
	{
		CSE_Abstract*		D	= F_entity_Create("wpn_rpg7_missile");
		R_ASSERT			(D);
		CSE_Temporary		*l_tpTemporary = dynamic_cast<CSE_Temporary*>(D);
		R_ASSERT							(l_tpTemporary);
		l_tpTemporary->m_tNodeID	= level_vertex_id();
		// Fill
		strcpy				(D->s_name,"wpn_rpg7_missile");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
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

	if(iAmmoElapsed && !m_pGrenade) 
	{
		CSE_Abstract*		D	= F_entity_Create("wpn_rpg7_missile");
		R_ASSERT			(D);
		CSE_Temporary		*l_tpTemporary = dynamic_cast<CSE_Temporary*>(D);
		R_ASSERT							(l_tpTemporary);
		l_tpTemporary->m_tNodeID	= level_vertex_id();
		l_tpTemporary->o_Position	= Position();
		// Fill
		strcpy				(D->s_name,"wpn_rpg7_missile");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}
void CWeaponRPG7::SwitchState(u32 S) 
{
	inherited::SwitchState(S);
	if(STATE == eIdle && S==eFire && m_pGrenade) 
	{
		Fvector						p1, d; p1.set(vLastFP); d.set(vLastFD);
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(this, p1,d);
		m_pGrenade->m_pos.set(p1);
		m_pGrenade->m_vel.set(d); m_pGrenade->m_vel.y += .0f; m_pGrenade->m_vel.mul(50.f);
		m_pGrenade->m_pOwner = dynamic_cast<CGameObject*>(H_Parent());
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16(m_pGrenade->ID()));
		u_EventSend(P);
	}
}

void CWeaponRPG7::FireStart()
{
	inherited::FireStart();
	//if(m_pGrenade && STATE==eIdle) {
	//	Fvector						p1, d; p1.set(vLastFP); d.set(vLastFD);
	//	CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
	//	if (E) E->g_fireParams		(p1,d);
	//	m_pGrenade->m_pos.set(p1);
	//	m_pGrenade->m_vel.set(d); m_pGrenade->m_vel.y += .0f; m_pGrenade->m_vel.mul(50.f);
	//	m_pGrenade->m_pOwner = dynamic_cast<CGameObject*>(H_Parent());
	//	NET_Packet P;
	//	u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
	//	P.w_u16(u16(m_pGrenade->ID()));
	//	u_EventSend(P);
	//}
}

void CWeaponRPG7::switch2_Fire	()
{
	VERIFY(fTimeToFire>0.f);

	if (fTime<=0)
	{
		///UpdateFP					();

		// Fire
		Fvector						p1, d; p1.set(vLastFP); d.set(vLastFD);
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(this, p1,d);

		//m_pGrenade->m_pos.set(p1);
		//m_pGrenade->m_vel.set(d); m_pGrenade->m_vel.y += .0f; m_pGrenade->m_vel.mul(50.f);
		//m_pGrenade->m_pOwner = dynamic_cast<CGameObject*>(H_Parent());
		//NET_Packet P;
		//u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		//P.w_u16(u16(m_pGrenade->ID()));
		//u_EventSend(P);

		OnShot						();
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire;

		CParticlesObject* pStaticPG;
		pStaticPG = xr_new<CParticlesObject>("weapons\\rpg_shoot_01",Sector());
		Fmatrix l_pos; 
		Fvector zero_vel = {0.f,0.f,0.f};
		l_pos.set(XFORM()); //l_pos.c.set(p1);
		pStaticPG->UpdateParent(l_pos, zero_vel); 
		pStaticPG->Play();

		// Patch for "previous frame position" :)))
		dwFP_Frame					= 0xffffffff;
		dwXF_Frame					= 0xffffffff;
	}
}

//void CWeaponRPG7::OnShot() {
//	inherited::OnShot();
//	R_ASSERT(m_pGrenade);
//	m_pGrenade->m_vel.set(0, 0, 0);
//	NET_Packet P;
//	u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
//	P.w_u16(u16(m_pGrenade->ID()));
//	u_EventSend(P);
//}

void CWeaponRPG7::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CWeaponRPG7Grenade *l_pG = dynamic_cast<CWeaponRPG7Grenade*>(Level().Objects.net_Find(id));
			m_pGrenade = l_pG;
			l_pG->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			CWeaponRPG7Grenade *l_pG = dynamic_cast<CWeaponRPG7Grenade*>(Level().Objects.net_Find(id));
			m_pGrenade = NULL;
			l_pG->H_SetParent(0);
		} break;
	}
}