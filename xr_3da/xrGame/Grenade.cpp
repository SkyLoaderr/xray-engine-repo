#include "stdafx.h"
#include "grenade.h"
#include "PhysicsShell.h"
#include "WeaponHUD.h"

CGrenade::CGrenade(void) {
	m_pFake = NULL;
}

CGrenade::~CGrenade(void) {}

BOOL CGrenade::net_Spawn(LPVOID DC) {
	return inherited::net_Spawn(DC);
}

void CGrenade::net_Destroy() {
	inherited::net_Destroy();
}

void CGrenade::OnH_A_Chield() {
	inherited::OnH_A_Chield();
	if(!m_pFake && !dynamic_cast<CGrenade*>(H_Parent())) {
		xrServerEntity*		D	= F_entity_Create(cNameSect());
		R_ASSERT			(D);
		// Fill
		strcpy				(D->s_name,cNameSect());
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

void CGrenade::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	//if(dynamic_cast<CGrenade*>(H_Parent())) inherited::OnH_B_Independent();
	//else CInventoryItem::OnH_B_Independent();
}

bool CGrenade::Activate() {
	Show();
	return true;
}

void CGrenade::Deactivate() {
	Hide();
}

void CGrenade::Throw() {
	//////inherited::Throw();
	//////m_destroyTime = 100000;
	//////if(m_pPhysicsShell) {
	//////	m_dropOnEnd = true;
	//////	setVisible					(true);
	//////	setEnabled					(true);
	//////	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	//////	R_ASSERT		(E);
	//////	svTransform.set(E->clXFORM());
	//////	vPosition.set(svTransform.c);
	//////	Fmatrix trans;
	//////	Level().Cameras.unaffected_Matrix(trans);
	//////	Fvector l_fw, l_r; l_fw.set(trans.k); l_r.set(30.f, 0, 0);
	//////	Fvector l_up; l_up.set(svTransform.j); l_up.mul(2.f);
	//////	Fmatrix l_p1, l_p2;
	//////	l_p1.set(svTransform); l_p1.c.add(l_up); l_up.mul(1.2f); //l_p1.c.add(l_fw);
	//////	l_p2.set(svTransform); l_p2.c.add(l_up); l_fw.mul(m_force); l_p2.c.add(l_fw);
	//////	//m_pPhysicsShell->Activate(l_p1, 0, l_p2);
	//////	Fvector l_vel,a_vel;
	//////	l_vel.add(l_up,l_fw);
	//////	a_vel.set(::Random.randF(2.f*M_PI,3.f*M_PI),::Random.randF(2.f*M_PI,3.f*M_PI),::Random.randF(2.f*M_PI,3.f*M_PI));
	//////	m_pPhysicsShell->Activate(l_p1, l_vel, a_vel);
	//////	svTransform.set(l_p1);
	//////	vPosition.set(svTransform.c);
	//////}
	if(!m_pFake) return;
	inherited::Throw();
	CGrenade *l_pG = m_pFake;
	if(l_pG) {
		l_pG->m_destroyTime = 7000;
		l_pG->m_force = m_force; m_force = 0;
		{
			NET_Packet P;
			u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
			P.w_u16(u16(l_pG->ID()));
			u_EventSend(P);
		}
	}
}

bool CGrenade::Useful() {
	// ≈сли IItem еще не полностью использованый, вернуть true
	return m_destroyTime == 0xffffffff;
}

void CGrenade::OnEvent(NET_Packet& P, u16 type) {
	inherited::OnEvent(P,type);
	u16 id;
	P.r_u16(id);
	CGrenade *l_pG = dynamic_cast<CGrenade*>(Level().Objects.net_Find(id));
	if(l_pG) switch (type) {
		case GE_OWNERSHIP_TAKE : {
			m_pFake = l_pG;
			l_pG->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			m_pFake = NULL;
			l_pG->H_SetParent(0);
		} break;
	}
}

void CGrenade::OnAnimationEnd() {
	switch(State()) {
		case MS_END : {
			m_destroyTime = 0;
			NET_Packet P;
			u_EventGen(P, GE_OWNERSHIP_REJECT, H_Parent()->ID());
			P.w_u16(u16(ID()));
			u_EventSend(P);
		} break;
		default : inherited::OnAnimationEnd();
	}
}
