////////////////////////////////////////////////////////////////////////////
//	Module 		: group_hierarchy_holder.cpp
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Group hierarchy holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "group_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "memory_manager.h"
#include "entity.h"
#include "agent_manager.h"
#include "ai/stalker/ai_stalker.h"

CGroupHierarchyHolder::~CGroupHierarchyHolder	()
{
	VERIFY						(!m_agent_manager);
}

void CGroupHierarchyHolder::register_member		(CEntity *member)
{
	VERIFY						(member);
	MEMBER_REGISTRY::iterator	I = std::find(m_members.begin(),m_members.end(),member);
	VERIFY3						(I == m_members.end(),"Specified group member has already been found",*member->cName());
	squad().inc_member_count	();

	if (!leader() && member->g_Alive()) {
		m_leader				= member;
		if (!squad().leader())
			squad().leader		(member);
	}

	if (!get_agent_manager() && dynamic_cast<CAI_Stalker*>(member)) {
		m_agent_manager						= xr_new<CAgentManager>();
		agent_manager().set_squad_objects	(&squad().visible_objects());
		agent_manager().set_squad_objects	(&squad().sound_objects());
		agent_manager().set_squad_objects	(&squad().hit_objects());
	}

	if (get_agent_manager())
		agent_manager().add		(member);

	m_members.push_back			(member);

	CMemoryManager				*memory_manager = dynamic_cast<CMemoryManager*>(member);
	if (memory_manager) {
		memory_manager->CVisualMemoryManager::set_squad_objects	(&squad().visible_objects());
		memory_manager->CSoundMemoryManager::set_squad_objects	(&squad().sound_objects());
		memory_manager->CHitMemoryManager::set_squad_objects	(&squad().hit_objects());
	}
}

void CGroupHierarchyHolder::unregister_member	(CEntity *member)
{
	VERIFY						(member);
	MEMBER_REGISTRY::iterator	I = std::find(m_members.begin(),m_members.end(),member);
	VERIFY3						(I != m_members.end(),"Specified group member cannot be found",*member->cName());
	m_members.erase				(I);
	squad().dec_member_count	();

	if (leader() && (leader()->ID() == member->ID())) {
		update_leader					();
		if (squad().leader()->ID() == member->ID())
			if (leader())
				squad().leader			(leader());
			else
				squad().update_leader	();
	}
	
	if (get_agent_manager())
		agent_manager().remove	(member);

	if (m_members.empty() && get_agent_manager())
		xr_delete				(m_agent_manager);
}

void CGroupHierarchyHolder::update_leader		()
{
	m_leader					= 0;
	MEMBER_REGISTRY::iterator	I = m_members.begin();
	MEMBER_REGISTRY::iterator	E = m_members.end();
	for ( ; I != E; ++I)
		if ((*I)->g_Alive()) {
			m_leader			= *I;
			break;
		}
}
