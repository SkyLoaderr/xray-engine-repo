#include "stdafx.h"
#include "bolt.h"

CBolt::CBolt(void) {
	//m_belt = true;
	m_weight = .1f;
	m_slot = 5;
	m_ruck = false;
}

CBolt::~CBolt(void) {}

void CBolt::OnH_A_Chield() {
	inherited::OnH_A_Chield();
	if(!m_subs.size() && !dynamic_cast<CBolt*>(H_Parent())) {
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

void CBolt::OnEvent(NET_Packet& P, u16 type) {
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CBolt *l_pB = dynamic_cast<CBolt*>(Level().Objects.net_Find(id));
			Attach(l_pB, true);
			l_pB->H_SetParent(this);
		} break;
		case GE_OWNERSHIP_REJECT : {
			P.r_u16(id);
			CBolt *l_pB = dynamic_cast<CBolt*>(Level().Objects.net_Find(id));
			Detach(l_pB);
			l_pB->H_SetParent(0);
		} break;
	}
}

bool CBolt::Activate() {
	Show();
	return true;
}

void CBolt::Deactivate() {
	Hide();
}

bool CBolt::Attach(PIItem pIItem, bool force) {
	if(!strcmp(cNameSect(), pIItem->cNameSect())) { force = true;}
	return inherited::Attach(pIItem, force);
}

void CBolt::Throw() {
	if(!m_subs.size()) return;
	inherited::Throw();
	CBolt *l_pBolt = dynamic_cast<CBolt*>(*m_subs.begin());
	if(l_pBolt) {
		//Detach(l_pBolt);
		l_pBolt->m_destroyTime = 100000;
		l_pBolt->m_force = m_force;
		//l_pBolt->Drop();
		{
			NET_Packet				P;
			u_EventGen				(P,GE_OWNERSHIP_REJECT,/*H_Parent()->*/ID());
			P.w_u16					(u16(l_pBolt->ID()));
			u_EventSend				(P);
		}

		// Спавним болты, чтоб у актера всегда было 5 болтов
		u32 l_c = 2 - m_subs.size();
		while(l_c--) {
			xrServerEntity*		D	= F_entity_Create(cNameSect());
			R_ASSERT			(D);
			// Fill
			strcpy				(D->s_name,cNameSect());
			strcpy				(D->s_name_replace,"");
			D->s_gameid			=	u8(GameID());
			D->s_RP				=	0xff;
			D->ID				=	0xffff;
			D->ID_Parent		=	(u16)/*H_Parent()->*/ID();
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
		//
	}
}

bool CBolt::Useful() {
	// Если IItem еще не полностью использованый, вернуть true
	return m_destroyTime == 0xffffffff;
}

bool CBolt::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kDROP : {
			if(flags&CMD_START) {
				 m_throw = false;
				if(State() == MS_IDLE) State(MS_THREATEN);
			} else if(State() == MS_READY || State() == MS_THREATEN) {
				m_throw = true; if(State() == MS_READY) State(MS_THROW);
			}
		} return true;
	}
	return false;
}
