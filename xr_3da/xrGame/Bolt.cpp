#include "stdafx.h"
#include "bolt.h"

CBolt::CBolt(void) {
	//m_belt = true;
	m_weight = .1f;
	m_slot = 5;
}

CBolt::~CBolt(void) {}

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
		Detach(l_pBolt);
		l_pBolt->m_destroyTime = 100000;
		l_pBolt->m_force = m_force;
		//l_pBolt->Drop();
		{
			NET_Packet				P;
			u_EventGen				(P,GE_OWNERSHIP_REJECT,H_Parent()->ID());
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
			D->ID_Parent		=	(u16)H_Parent()->ID();
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
