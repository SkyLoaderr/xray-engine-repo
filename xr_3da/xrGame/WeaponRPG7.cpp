#include "stdafx.h"
#include "weaponrpg7.h"
#include "WeaponHUD.h"
#include "entity.h"

CWeaponRPG7Grenade::CWeaponRPG7Grenade() {
	m_pos.set(0, 0, 0); m_vel.set(0, 0, 0);
}

CWeaponRPG7Grenade::~CWeaponRPG7Grenade() {}

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

BOOL CWeaponRPG7Grenade::net_Spawn(LPVOID DC) {
	BOOL l_res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");
	//setVisible					(true);
	//setEnabled					(true);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
					)
		//radius*=1.4142f;
		Fsphere sphere1,sphere2;
		sphere1.P.add						(obb.m_translate,ax);
		sphere1.R							=radius*1.4142f;

		sphere2.P.sub						(obb.m_translate,ax);
		sphere2.R							=radius/2.f;

		E->add_Box							(obb);
		E->add_Sphere						(sphere1);
		E->add_Sphere						(sphere2);
	
		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setMass			(4000.f);
		//m_pPhysicsShell->Activate			(svXFORM(),0,svXFORM());
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		//m_pPhysicsShell->Deactivate();
	}
	return l_res;
}

void CWeaponRPG7Grenade::net_Destroy() {
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
}

void CWeaponRPG7Grenade::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		//Fvector l_fw; l_fw.set(trans.k);
		//Fvector l_up; l_up.set(svTransform.j); l_up.mul(2.f);
		Fmatrix l_p1, l_r; l_r.rotateY(M_PI*-.5); l_p1.mul(trans, l_r); l_p1.c.set(m_pos);
		//l_p1.set(svTransform); l_p1.c.add(l_up); l_up.mul(1.2f);
		//l_p2.set(svTransform); l_p2.c.add(l_up); l_fw.mul(1.f); l_p2.c.add(l_fw);
		Fvector a_vel; a_vel.set(0, 0, 0);
		//float fi,teta,r;
		//l_vel.add(l_up,l_fw);
		//fi=  ::Random.randF(0.f,2.f*M_PI);
		//teta=::Random.randF(0.f,M_PI);
		//r=	 ::Random.randF(2.f*M_PI,3.f*M_PI);
		//float rxy=r*_sin(teta);
		//a_vel.set(rxy*_cos(fi),rxy*_sin(fi),r*_cos(teta));
		////a_vel.set(::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI),::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI),::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI));
		m_pPhysicsShell->Activate(l_p1, m_vel, a_vel);
		svTransform.set(m_pPhysicsShell->mXFORM);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
		//svTransform.set(l_p1);
		//vPosition.set(svTransform.c);
	}
}

void CWeaponRPG7Grenade::UpdateCL() {
	inherited::UpdateCL();
	if(getVisible() && m_pPhysicsShell) {
		//if(m_destroyTime < Device.dwTimeDelta) {
		//	m_destroyTime = 0xffffffff;
		//	R_ASSERT(!m_pInventory);
		//	Destroy();
		//	return;
		//}
		//if(m_destroyTime < 0xffffffff) m_destroyTime -= Device.dwTimeDelta;
		m_pPhysicsShell->Update	();
		svTransform.set(m_pPhysicsShell->mXFORM);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
	} //else if(H_Parent()) svTransform.set(H_Parent()->clXFORM());
}

CWeaponRPG7::CWeaponRPG7(void) : CWeaponCustomPistol("RPG7") {
	m_weight = 5.f;
	m_slot = 2;
	m_hideGrenade = false;
	m_pGrenade = NULL;
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

void CWeaponRPG7::ReloadMagazine() {
	inherited::ReloadMagazine();
	if(iAmmoElapsed && !m_pGrenade) {
		xrServerEntity*		D	= F_entity_Create("wpn_rpg7_messile");
		R_ASSERT			(D);
		// Fill
		strcpy				(D->s_name,"wpn_rpg7_messile");
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	(u16)ID();
		D->ID_Phantom		=	0xffff;
		D->s_flags.set		(M_SPAWN_OBJECT_ACTIVE | M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
}

void CWeaponRPG7::switch2_Fire	()
{
	if (fTime<=0)
	{
		UpdateFP					();

		// Fire
		Fvector						p1, d; p1.set(vLastFP); d.set(vLastFD);
		CEntity*					E = dynamic_cast<CEntity*>(H_Parent());
		if (E) E->g_fireParams		(p1,d);

		m_pGrenade->m_pos.set(p1);
		m_pGrenade->m_vel.set(d); m_pGrenade->m_vel.mul(100.f);
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16(u16(m_pGrenade->ID()));
		u_EventSend(P);

		bFlame						=	TRUE;
		OnShot						();
		FireTrace					(p1,vLastFP,d);
		fTime						+= fTimeToFire;

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

void CWeaponRPG7::OnEvent(NET_Packet& P, u16 type) {
	inherited::OnEvent(P,type);
	u16 id;
	P.r_u16(id);
	CWeaponRPG7Grenade *l_pG = dynamic_cast<CWeaponRPG7Grenade*>(Level().Objects.net_Find(id));
	if(l_pG) switch (type) {
		case GE_OWNERSHIP_TAKE : {
			m_pGrenade = l_pG;
			l_pG->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			m_pGrenade = NULL;
			l_pG->H_SetParent(0);
		} break;
	}
}
