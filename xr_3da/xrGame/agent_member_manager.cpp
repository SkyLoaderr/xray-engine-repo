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

void CAgentMemberManager::add					(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker || !stalker->g_Alive())
		return;

	VERIFY2						(sizeof(squad_mask_type)*8 > members().size(),"Too many stalkers in squad!");

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I == m_members.end());
	m_members.push_back			(CMemberOrder(stalker));
}

void CAgentMemberManager::remove				(CEntity *member)
{
	CAI_Stalker					*stalker = smart_cast<CAI_Stalker*>(member);
	if (!stalker)
		return;

	unregister_in_combat		(stalker);

	iterator					I = std::find_if(m_members.begin(),m_members.end(), CMemberPredicate(stalker));
	VERIFY						(I != m_members.end());
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
	m_combat_mask				|= mask(object);
}

void CAgentMemberManager::unregister_in_combat	(const CAI_Stalker *object)
{
	m_combat_mask				&= squad_mask_type(-1) ^ mask(object);
}

bool CAgentMemberManager::registered_in_combat	(const CAI_Stalker *object) const
{
	return						(!!(m_combat_mask & mask(object)));
}
