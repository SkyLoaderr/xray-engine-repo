#include "stdafx.h"
#include "game_sv_single.h"
#include "ai_alife.h"

game_sv_Single::~game_sv_Single			()
{
	xr_delete							(m_tpALife);
}

void	game_sv_Single::Create			(LPSTR &options)
{
	if (strstr(options,"/alife")) {
		m_tpALife						= xr_new<CSE_ALifeSimulator>(m_tpServer);
		m_tpALife->m_cppServerOptions	= &options;
		string64						S;
		strcpy							(S,*m_tpALife->m_cppServerOptions);
		LPSTR							l_cpPointer = strchr(S,'/');
		R_ASSERT2						(l_cpPointer,"Invalid server options!");
		*l_cpPointer					= 0;
		m_tpALife->Load					(S);
	}

	switch_Phase		(GAME_PHASE_PENDING);
}

CSE_Abstract*		game_sv_Single::get_entity_from_eid		(u16 id)
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
	CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

	if (m_tpALife) {
		CSE_ALifeTraderAbstract	*tpTraderParams		= dynamic_cast<CSE_ALifeTraderAbstract*>(e_who);
		CSE_ALifeItem			*tpALifeItem		= dynamic_cast<CSE_ALifeItem*>			(e_what);
		CSE_ALifeDynamicObject	*tpDynamicObject	= dynamic_cast<CSE_ALifeDynamicObject*>	(e_who);
		
		if (tpTraderParams && tpALifeItem && tpDynamicObject && (m_tpALife->m_tObjectRegistry.find(e_who->ID) != m_tpALife->m_tObjectRegistry.end()) && (m_tpALife->m_tObjectRegistry.find(tpALifeItem->ID) != m_tpALife->m_tObjectRegistry.end()))
			m_tpALife->vfAttachItem(*e_who,tpALifeItem,tpALifeItem->m_tGraphID,false);
	}
	return TRUE;
}

BOOL	game_sv_Single::OnDetach		(u16 eid_who, u16 eid_what)
{
	if (m_tpALife) {
		CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
		CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

		CSE_ALifeItem *tpALifeItem = dynamic_cast<CSE_ALifeItem*>(e_what);
		if (!tpALifeItem)
			return TRUE;

		CSE_ALifeDynamicObject *tpDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(e_who);
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
//		m_tpALife				= xr_new<CSE_ALifeSimulator>(m_tpServer);
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
	//xr_vector<u16>*	C				=	get_children(id_killed);
	//if (0==C)						return;
	//while(C->size())
	//{
	//	u16		eid						= (*C)[0];

	//	CSE_Abstract*		from		= S->get_entity_from_eid(get_id_2_eid(id_killed));
	//	CSE_Abstract*		what		= S->get_entity_from_eid(eid);
	//	S->Perform_reject				(what,from);
	//}
}
