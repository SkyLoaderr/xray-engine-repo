#include "stdafx.h"
#include "game_sv_single.h"
#include "ai_alife.h"

game_sv_Single::~game_sv_Single			()
{
	xr_delete							(m_tpALife);
}

void	game_sv_Single::Create			(LPSTR &options)
{
	inherited::Create					(options);
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
		D_OBJECT_PAIR_IT	I = m_tpALife->m_tObjectRegistry.find(id);
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
		CSE_ALifeTraderAbstract	*l_tpTraderParams		= dynamic_cast<CSE_ALifeTraderAbstract*>(e_who);
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem	= dynamic_cast<CSE_ALifeInventoryItem*>	(e_what);
		CSE_ALifeDynamicObject	*l_tpDynamicObject		= dynamic_cast<CSE_ALifeDynamicObject*>	(e_who);
		
		if (l_tpTraderParams && l_tpALifeInventoryItem && l_tpDynamicObject && (m_tpALife->m_tpCurrentLevel->find(l_tpALifeInventoryItem->ID) != m_tpALife->m_tpCurrentLevel->end()) && (m_tpALife->m_tObjectRegistry.find(e_who->ID) != m_tpALife->m_tObjectRegistry.end()) && (m_tpALife->m_tObjectRegistry.find(e_what->ID) != m_tpALife->m_tObjectRegistry.end()))
			m_tpALife->vfAttachItem(*e_who,l_tpALifeInventoryItem,l_tpDynamicObject->m_tGraphID,false);
#ifdef DEBUG
		else
			if (psAI_Flags.test(aiALife)) {
				Msg				("Cannot attach object [%s][%d] to object [%s][%d]",l_tpALifeInventoryItem->s_name_replace,l_tpALifeInventoryItem->ID,l_tpDynamicObject->s_name_replace,l_tpDynamicObject->ID);
			}
#endif
	}
	return TRUE;
}

BOOL	game_sv_Single::OnDetach		(u16 eid_who, u16 eid_what)
{
	if (m_tpALife) {
		CSE_Abstract*		e_who	= get_entity_from_eid(eid_who);		VERIFY(e_who	);
		CSE_Abstract*		e_what	= get_entity_from_eid(eid_what);	VERIFY(e_what	);

		CSE_ALifeInventoryItem *l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(e_what);
		if (!l_tpALifeInventoryItem)
			return TRUE;

		CSE_ALifeDynamicObject *l_tpDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(e_who);
		if (!l_tpDynamicObject)
			return TRUE;
		
		if ((m_tpALife->m_tObjectRegistry.find(e_who->ID) != m_tpALife->m_tObjectRegistry.end()) && (m_tpALife->m_tpCurrentLevel->find(l_tpALifeInventoryItem->ID) == m_tpALife->m_tpCurrentLevel->end()) && (m_tpALife->m_tObjectRegistry.find(e_who->ID) != m_tpALife->m_tObjectRegistry.end()) && (m_tpALife->m_tObjectRegistry.find(e_what->ID) != m_tpALife->m_tObjectRegistry.end()))
			m_tpALife->vfDetachItem(*e_who,l_tpALifeInventoryItem,l_tpDynamicObject->m_tGraphID,false);
#ifdef DEBUG
		else
			if (psAI_Flags.test(aiALife)) {
				Msg			("Cannot detach object [%s][%d] to object [%s][%d]",l_tpALifeInventoryItem->s_name_replace,l_tpALifeInventoryItem->ID,l_tpDynamicObject->s_name_replace,l_tpDynamicObject->ID);
			}
#endif
	}
	return					(TRUE);
}

void	game_sv_Single::OnRoundStart	()
{
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
}

_TIME_ID game_sv_Single::GetGameTime		()
{
	if (m_tpALife && m_tpALife->m_bLoaded)
		return(m_tpALife->tfGetGameTime());
	else
		return(inherited::GetGameTime());
}

float game_sv_Single::GetGameTimeFactor		()
{
	if (m_tpALife && m_tpALife->m_bLoaded)
		return(m_tpALife->m_fTimeFactor);
	else
		return(1.f);
}