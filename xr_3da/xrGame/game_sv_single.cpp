#include "stdafx.h"
#include "game_sv_single.h"

void	game_sv_Single::Create			(LPCSTR options)
{
	m_bALife			= !!strstr(options,"/alife");
	switch_Phase		(GAME_PHASE_PENDING);
}
xrServerEntity*		game_sv_Single::get_entity_from_eid		(u16 id)
{
	if (m_tpALife) {
		OBJECT_PAIR_IT	I = m_tpALife->m_tObjectRegistry.find(id);
		if (I != m_tpALife->m_tObjectRegistry.end())
			return((*I).second);
		else
			return(inherited::get_entity_from_eid(id));
	}
	else
		return(inherited::get_entity_from_eid(id));
}

BOOL	game_sv_Single::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServerEntity*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
	xrServerEntity*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

	if (m_bALife) {
		
		CALifeTraderParams	*tpTraderParams		= dynamic_cast<CALifeTraderParams*>	(e_who);
		CALifeItem			*tpALifeItem		= dynamic_cast<CALifeItem*>			(e_what);
		CALifeDynamicObject *tpDynamicObject	= dynamic_cast<CALifeDynamicObject*>(e_who);
		
		if (tpTraderParams && tpALifeItem && tpDynamicObject && (m_tpALife->m_tObjectRegistry.find(e_who->ID) != m_tpALife->m_tObjectRegistry.end()) && (m_tpALife->m_tObjectRegistry.find(tpALifeItem->ID) != m_tpALife->m_tObjectRegistry.end()))
			m_tpALife->vfAttachItem(*e_who,tpALifeItem,tpALifeItem->m_tGraphID,false);
	}
	return TRUE;
}

BOOL	game_sv_Single::OnDetach		(u16 eid_who, u16 eid_what)
{
	if (m_bALife) {
		xrServerEntity*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
		xrServerEntity*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

		CALifeItem *tpALifeItem = dynamic_cast<CALifeItem*>(e_what);
		if (!tpALifeItem)
			return TRUE;

		CALifeDynamicObject *tpDynamicObject = dynamic_cast<CALifeDynamicObject*>(e_who);
		if (!tpDynamicObject)
			return TRUE;
		
		if ((m_tpALife->m_tObjectRegistry.find(e_who->ID) != m_tpALife->m_tObjectRegistry.end()) && (m_tpALife->m_tObjectRegistry.find(tpALifeItem->ID) != m_tpALife->m_tObjectRegistry.end()))
			m_tpALife->vfDetachItem(*e_who,tpALifeItem,tpDynamicObject->m_tGraphID,false);
	}
	return					(TRUE);
}

void	game_sv_Single::OnRoundStart	()
{
//	if (m_bALife) {
//		m_tpALife				= xr_new<CAI_ALife>(m_tpServer);
//		m_tpALife->Load			();
//	}
}

void	game_sv_Single::Update			()
{
	__super::Update	();
	switch(phase) 	{
		case GAME_PHASE_PENDING : {
			OnRoundStart();
			switch_Phase(GAME_PHASE_INPROGRESS);
			break;
		}
	}
}

void	game_sv_Single::OnPlayerKillPlayer	(u32 id_killer, u32 id_killed)
{
	//xrServer*	S					=	Level().Server;
	//// Drop everything
	//vector<u16>*	C				=	get_children(id_killed);
	//if (0==C)						return;
	//while(C->size())
	//{
	//	u16		eid						= (*C)[0];

	//	xrServerEntity*		from		= S->get_entity_from_eid(get_id_2_eid(id_killed));
	//	xrServerEntity*		what		= S->get_entity_from_eid(eid);
	//	S->Perform_reject				(what,from);
	//}
}
