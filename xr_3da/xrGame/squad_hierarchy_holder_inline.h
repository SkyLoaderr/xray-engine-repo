////////////////////////////////////////////////////////////////////////////
//	Module 		: squad_hierarchy_holder_inline.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Squad hierarchy holder inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSquadHierarchyHolder::CSquadHierarchyHolder									(CTeamHierarchyHolder *team)
{
	VERIFY						(team);
	m_team						= team;
	m_visible_objects			= 0;
	m_sound_objects				= 0;
	m_hit_objects				= 0;
	m_leader					= 0;
	m_member_count				= 0;
	
	m_groups.resize				(max_group_count);
	GROUP_REGISTRY::iterator	I = m_groups.begin();
	GROUP_REGISTRY::iterator	E = m_groups.end();
	for ( ; I != E; ++I)
		*I						= 0;
}

IC	CEntity	*CSquadHierarchyHolder::leader											() const
{
	return						(m_leader);
}

IC	CTeamHierarchyHolder &CSquadHierarchyHolder::team								() const
{
	VERIFY						(m_team);
	return						(*m_team);
}

IC	u32	 CSquadHierarchyHolder::member_count										() const
{
	return						(m_member_count);
}

IC	void CSquadHierarchyHolder::leader												(CEntity *leader)
{
	m_leader					= leader;
}

IC	SquadHierarchyHolder::VISIBLE_OBJECTS &CSquadHierarchyHolder::visible_objects	() const
{
	VERIFY						(m_visible_objects);
	return						(*m_visible_objects);
}

IC	SquadHierarchyHolder::SOUND_OBJECTS &CSquadHierarchyHolder::sound_objects		() const
{
	VERIFY						(m_sound_objects);
	return						(*m_sound_objects);
}

IC	SquadHierarchyHolder::HIT_OBJECTS &CSquadHierarchyHolder::hit_objects			() const
{
	VERIFY						(m_hit_objects);
	return						(*m_hit_objects);
}

IC	const SquadHierarchyHolder::GROUP_REGISTRY &CSquadHierarchyHolder::groups		() const
{
	return						(m_groups);
}
