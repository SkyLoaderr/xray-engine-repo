////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point_inline.h
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const Fvector &CPatrolPoint::position	() const
{
	VERIFY				(m_initialized);
	return				(m_position);
}

IC	u32	 CPatrolPoint::level_vertex_id		() const
{
	VERIFY				(m_initialized);
	return				(m_level_vertex_id);
}

IC	u32	 CPatrolPoint::flags				() const
{
	VERIFY				(m_initialized);
	return				(m_flags);
}

IC	shared_str	CPatrolPoint::name				() const
{
	VERIFY				(m_initialized);
	return				(m_name);
}
