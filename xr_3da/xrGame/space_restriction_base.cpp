////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_base.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction base
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction_base.h"
#include "ai_space.h"
#include "level_graph.h"

#define DEFAULT_RADIUS EPS_L

bool CSpaceRestrictionBase::inside	(u32 level_vertex_id, bool partially_inside)
{
	return							(inside(level_vertex_id,partially_inside,DEFAULT_RADIUS));
}

IC	Fvector construct_position		(u32 level_vertex_id, float x, float z)
{
	return							(Fvector().set(x,ai().level_graph().vertex_plane_y(level_vertex_id,x,z),z));
}

bool CSpaceRestrictionBase::inside	(u32 level_vertex_id, bool partially_inside, float radius)
{
	const Fvector					&position = ai().level_graph().vertex_position(level_vertex_id);
	float							offset = ai().level_graph().header().cell_size()*.5f - EPS_L;
	if (partially_inside)
		return						(
			inside(construct_position(level_vertex_id,position.x + offset,position.z + offset),radius) || 
			inside(construct_position(level_vertex_id,position.x + offset,position.z - offset),radius) ||
			inside(construct_position(level_vertex_id,position.x - offset,position.z + offset),radius) || 
			inside(construct_position(level_vertex_id,position.x - offset,position.z - offset),radius) ||
			inside(Fvector().set(position.x,position.y,position.z),radius)
		);
	else
		return						(
			inside(construct_position(level_vertex_id,position.x + offset,position.z + offset),radius) && 
			inside(construct_position(level_vertex_id,position.x + offset,position.z - offset),radius) && 
			inside(construct_position(level_vertex_id,position.x - offset,position.z + offset),radius) && 
			inside(construct_position(level_vertex_id,position.x - offset,position.z - offset),radius) &&
			inside(Fvector().set(position.x,position.y,position.z),radius)
		);
}

bool CSpaceRestrictionBase::inside			(const Fvector &position)
{
	return							(inside(position,DEFAULT_RADIUS));
}
