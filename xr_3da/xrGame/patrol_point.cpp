////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_point.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol point
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_point.h"
#include "ai_space.h"
#include "level_graph.h"

CPatrolPoint::CPatrolPoint				()
{
#ifdef DEBUG
	m_initialized		= false;
#endif
}

IC	void CPatrolPoint::correct_position	()
{
	if (!ai().get_level_graph())
		return;

	if (!ai().level_graph().inside(level_vertex_id(),position()))
		m_position		= ai().level_graph().vertex_position(level_vertex_id());
}

CPatrolPoint::CPatrolPoint				(const Fvector &position, u32 level_vertex_id, u32 flags, shared_str name)
{
	m_position			= position;
	m_level_vertex_id	= level_vertex_id;
	m_flags				= flags;
	m_name				= name;
#ifdef DEBUG
	m_initialized		= true;
#endif
	correct_position	();
}

CPatrolPoint &CPatrolPoint::load		(IReader &stream)
{
	stream.r_fvector3	(m_position);
	m_flags				= stream.r_u32();
	stream.r_stringZ	(m_name);
	if (ai().get_level_graph())
		m_level_vertex_id	= ai().level_graph().vertex(u32(-1),m_position);
	else
		m_level_vertex_id	= u32(-1);
#ifdef DEBUG
	m_initialized		= true;
#endif
	correct_position	();
	return				(*this);
}
