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

bool CSpaceRestrictionBase::inside			(u32 level_vertex_id, bool out, float radius)
{
	const Fvector					&position = ai().level_graph().vertex_position(level_vertex_id);
	float							offset = ai().level_graph().header().cell_size()*.5f - 10*flt_eps;
	if (out)
		return							(
			inside(Fvector().set(position.x + offset,position.y,position.z + offset),radius) || 
			inside(Fvector().set(position.x + offset,position.y,position.z - offset),radius) || 
			inside(Fvector().set(position.x - offset,position.y,position.z + offset),radius) || 
			inside(Fvector().set(position.x - offset,position.y,position.z - offset),radius) ||
			inside(Fvector().set(position.x,position.y,position.z),radius)
		);
	else
		return							(
			inside(Fvector().set(position.x + offset,position.y,position.z + offset),radius) && 
			inside(Fvector().set(position.x + offset,position.y,position.z - offset),radius) && 
			inside(Fvector().set(position.x - offset,position.y,position.z + offset),radius) && 
			inside(Fvector().set(position.x - offset,position.y,position.z - offset),radius) &&
			inside(Fvector().set(position.x,position.y,position.z),radius)
		);
}

u32	CSpaceRestrictionBase::accessible_nearest	(const Fvector &position, Fvector &result, bool out_restriction)
{
#pragma todo("Dima to Dima : _Warning : this place can be optimized in case of a slowdown")
	VERIFY							(initialized());
	float	min_dist_sqr = flt_max;
	u32		selected = u32(-1);
	xr_vector<u32>::const_iterator	I = border(out_restriction).begin();
	xr_vector<u32>::const_iterator	E = border(out_restriction).end();
	for ( ; I != E; ++I) {
		float	distance_sqr = ai().level_graph().vertex_position(*I).distance_to_sqr(position);
		if (distance_sqr < min_dist_sqr) {
			min_dist_sqr	= distance_sqr;
			selected		= *I;
		}
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));
	{
		min_dist_sqr = flt_max;
		u32	new_selected = u32(-1);
		CLevelGraph::const_iterator	I, E;
		ai().level_graph().begin(selected,I,E);
		for ( ; I != E; ++I) {
			u32	current = ai().level_graph().value(selected,I);
			if (!ai().level_graph().valid_vertex_id(current))
				continue;
			if (!inside(current,true))
				continue;
			float	distance_sqr = ai().level_graph().vertex_position(current).distance_to_sqr(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				new_selected = current;
			}
		}
		selected	= new_selected;
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));
	{
		Fvector		center = ai().level_graph().vertex_position(selected);
		float		offset = ai().level_graph().header().cell_size()*.5f - 10*flt_eps;
		bool		found = false;
		min_dist_sqr = flt_max;
		for (u32 i=0; i<5; ++i) {
#ifndef DEBUG
			Fvector	current;
#else
			Fvector	current = Fvector().set(flt_max,flt_max,flt_max);
#endif
			switch (i) {
				case 0 : current.set(center.x + offset,center.y,center.z + offset); break;
				case 1 : current.set(center.x + offset,center.y,center.z - offset); break;
				case 2 : current.set(center.x - offset,center.y,center.z + offset); break;
				case 3 : current.set(center.x - offset,center.y,center.z - offset); break;
				case 4 : current.set(center.x,center.y,center.z); break;
				default : NODEFAULT;
			}
			if (i < 4)
				center.y = ai().level_graph().vertex_plane_y(selected,center.x,center.z);
			if (!inside(current))
				continue;
			float	distance_sqr = current.distance_to(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				result = current;
				found = true;
			}
		}
		VERIFY	(found);
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));
	return	(selected);
}
