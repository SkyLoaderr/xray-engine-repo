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
#include "level_navigation_graph.h"

#ifdef DEBUG
#	include "patrol_path.h"
#endif

CPatrolPoint::CPatrolPoint				(const CPatrolPath *path)
{
#ifdef DEBUG
	m_path				= path;
	m_initialized		= false;
#endif
}

#ifdef DEBUG
void CPatrolPoint::verify_vertex_id		() const
{
	if (ai().level_graph().valid_vertex_id(m_level_vertex_id))
		return;

	VERIFY			(m_path);
	string1024		temp;
	sprintf			(temp,"\n! Patrol point %s in path %s is not on the level graph vertex!",*m_name,*m_path->m_name);
	THROW2			(ai().level_graph().valid_vertex_id(m_level_vertex_id),temp);
}
#endif

IC	void CPatrolPoint::correct_position	()
{
	if (!ai().get_level_graph() || !ai().level_graph().valid_vertex_position(position()) || !ai().level_graph().valid_vertex_id(m_level_vertex_id))
		return;

	if (!ai().level_graph().inside(level_vertex_id(),position()))
		m_position		= ai().level_graph().vertex_position(level_vertex_id());
}

CPatrolPoint::CPatrolPoint				(const CPatrolPath *path, const Fvector &position, u32 level_vertex_id, u32 flags, shared_str name)
{
#ifdef DEBUG
	VERIFY				(path);
	m_path				= path;
#endif
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
	if (ai().get_level_graph() && ai().level_graph().valid_vertex_position(m_position)) {
		Fvector				position = m_position;
		position.y			+= .15f;
		m_level_vertex_id	= ai().level_graph().vertex_id(position);
	}
	else
		m_level_vertex_id	= u32(-1);
#ifdef DEBUG
	m_initialized		= true;
#endif
	correct_position	();
	return				(*this);
}
