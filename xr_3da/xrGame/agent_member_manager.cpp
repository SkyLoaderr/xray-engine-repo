////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_member_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent member manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_member_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "object_broker.h"
#include "agent_manager.h"
#include "agent_memory_manager.h"

CAgentMemberManager::~CAgentMemberManager		()
{
	delete_data					(m_members);
}

void CAgentMemberManager::add					(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker || !stalker->g_Alive())
		return;

	VERIFY2						(sizeof(squad_mask_type)*8 > members().size(),"Too many stalkers in squad!");

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I == m_members.end());
	m_members.push_back			(xr_new<CMemberOrder>(stalker));

	//. temporary until full strength stealth mode is not ready
	register_in_combat			(stalker);
}

void CAgentMemberManager::remove				(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;

	unregister_in_combat		(stalker);

	squad_mask_type							m = mask(stalker);
	object().memory().update_memory_masks	(m);
	object().memory().update_memory_mask	(m,m_combat_mask);

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I != m_members.end());
	xr_delete					(*I);
	m_members.erase				(I);
}

void CAgentMemberManager::update				()
{
}

void CAgentMemberManager::remove_links			(CObject *object)
{
}

void CAgentMemberManager::register_in_combat	(const CAI_Stalker *object)
{
	if (!object->group_behaviour())
		return;

	squad_mask_type				m = mask(object);
	m_actuality					= m_actuality && ((m_combat_mask | m) == m_combat_mask);
	m_combat_mask				|= m;
}

void CAgentMemberManager::unregister_in_combat	(const CAI_Stalker *object)
{
	if (!object->group_behaviour()) {
		VERIFY					(!registered_in_combat(object));
		return;
	}

	squad_mask_type				m = mask(object);
	m_actuality					= m_actuality && ((m_combat_mask & (squad_mask_type(-1) ^ m)) == m_combat_mask);
	m_combat_mask				&= squad_mask_type(-1) ^ m;
}

bool CAgentMemberManager::registered_in_combat	(const CAI_Stalker *object) const
{
	return						(!!(m_combat_mask & mask(object)));
}

CAgentMemberManager::MEMBER_STORAGE &CAgentMemberManager::combat_members	()
{
	if (m_actuality)
		return							(m_combat_members);

	m_actuality							= true;

	m_combat_members.clear				();
	MEMBER_STORAGE::iterator			I = members().begin();
	MEMBER_STORAGE::iterator			E = members().end();
	for ( ; I != E; ++I) {
		if (registered_in_combat(&(*I)->object()))
			m_combat_members.push_back	(*I);
	}

	return								(m_combat_members);
}
