////////////////////////////////////////////////////////////////////////////
//	Module 		: group_hierarchy_holder_inline.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Group hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CGroupHierarchyHolder::CGroupHierarchyHolder			(CSquadHierarchyHolder *squad)
{
	VERIFY				(squad);
	m_squad				= squad;
	m_leader			= 0;
	m_agent_manager		= 0;
	m_dwLastActionTime	= 0;
	m_dwLastAction		= 0;
	m_dwActiveCount		= 0;
	m_dwAliveCount		= 0;
	m_dwStandingCount	= 0;
}

IC	CAgentManager &CGroupHierarchyHolder::agent_manager		() const
{
	VERIFY			(m_agent_manager);
	return			(*m_agent_manager);
}

IC	CAgentManager *CGroupHierarchyHolder::get_agent_manager	() const
{
	return			(m_agent_manager);
}

IC	const GroupHierarchyHolder::MEMBER_REGISTRY &CGroupHierarchyHolder::members	() const
{
	return			(m_members);
}

IC	CSquadHierarchyHolder &CGroupHierarchyHolder::squad		() const
{
	VERIFY			(m_squad);
	return			(*m_squad);
}

IC	CEntity	*CGroupHierarchyHolder::leader					() const
{
	return			(m_leader);
}
