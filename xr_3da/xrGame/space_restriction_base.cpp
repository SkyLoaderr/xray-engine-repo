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

bool CSpaceRestrictionBase::inside(u32 level_vertex_id, bool partially_inside)
{
	return							(inside(level_vertex_id,partially_inside,DEFAULT_RADIUS));
}

bool CSpaceRestrictionBase::inside(u32 level_vertex_id, bool partially_inside, float radius)
{
	const Fvector					&position = ai().level_graph().vertex_position(level_vertex_id);
	float							offset = ai().level_graph().header().cell_size()*.5f - EPS;
	if (partially_inside)
		return						(
			inside(Fvector().set(position.x + offset,position.y,position.z + offset),radius) || 
			inside(Fvector().set(position.x + offset,position.y,position.z - offset),radius) ||
			inside(Fvector().set(position.x - offset,position.y,position.z + offset),radius) || 
			inside(Fvector().set(position.x - offset,position.y,position.z - offset),radius) ||
			inside(Fvector().set(position.x,position.y,position.z),radius)
		);
	else
		return						(
			inside(Fvector().set(position.x + offset,position.y,position.z + offset),radius) && 
			inside(Fvector().set(position.x + offset,position.y,position.z - offset),radius) && 
			inside(Fvector().set(position.x - offset,position.y,position.z + offset),radius) && 
			inside(Fvector().set(position.x - offset,position.y,position.z - offset),radius) &&
			inside(Fvector().set(position.x,position.y,position.z),radius)
		);
}

bool CSpaceRestrictionBase::inside			(const Fvector &position)
{
	return							(inside(position,DEFAULT_RADIUS));
}
