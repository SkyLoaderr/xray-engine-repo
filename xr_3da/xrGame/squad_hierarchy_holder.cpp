////////////////////////////////////////////////////////////////////////////
//	Module 		: squad_hierarchy_holder.cpp
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev, Oles Shishkovtsov, Aleksandr Maksimchuk
//	Description : Squad hierarchy holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"
#include "object_broker.h"
#include "seniority_hierarchy_space.h"
#include "memory_space.h"

CSquadHierarchyHolder::~CSquadHierarchyHolder		()
{
	VERIFY					(!m_member_count);
	VERIFY					(!m_visible_objects);
	VERIFY					(!m_sound_objects);
	VERIFY					(!m_hit_objects);
	delete_data				(m_groups);
}

CGroupHierarchyHolder &CSquadHierarchyHolder::group	(u32 group_id) const
{
	VERIFY3					(group_id < max_group_count,"Group id is invalid : ",*SeniorityHierarchy::to_string(group_id));
	if (!m_groups[group_id])
		m_groups[group_id]	= xr_new<CGroupHierarchyHolder>(const_cast<CSquadHierarchyHolder*>(this));
	return					(*m_groups[group_id]);
}

void CSquadHierarchyHolder::dec_member_count		()
{
	VERIFY					(m_member_count);
	--m_member_count;
	if (m_member_count)
		return;

	xr_delete				(m_visible_objects);
	xr_delete				(m_sound_objects);
	xr_delete				(m_hit_objects);
}

void CSquadHierarchyHolder::inc_member_count		()
{
	VERIFY					(m_member_count != u32(-1));

	if (!m_member_count) {
		m_visible_objects	= xr_new<VISIBLE_OBJECTS>();
		m_sound_objects		= xr_new<SOUND_OBJECTS>();
		m_hit_objects		= xr_new<HIT_OBJECTS>();
	}

	++m_member_count;
}

void CSquadHierarchyHolder::update_leader			()
{
	m_leader				= 0;
	GROUP_REGISTRY::const_iterator	I = m_groups.begin();
	GROUP_REGISTRY::const_iterator	E = m_groups.end();
	for ( ; I != E; ++I)
		if (*I && (*I)->leader()) {
			leader			((*I)->leader());
			break;
		}
}
