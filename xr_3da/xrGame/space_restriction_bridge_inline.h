////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_bridge_inline.h
//	Created 	: 27.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction bridge inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictionBridge::CSpaceRestrictionBridge		(CSpaceRestrictionBase *object)
{
	VERIFY		(object);
	m_object	= object;
}

IC	CSpaceRestrictionBase &CSpaceRestrictionBridge::object	() const
{
	VERIFY		(m_object);
	return		(*m_object);
}

template <typename T>
IC	bool CSpaceRestrictionBridge::accessible_neighbours	(T &restriction, u32 level_vertex_id, bool out_restriction)
{
	CLevelGraph::const_iterator	I, E;
	ai().level_graph().begin(level_vertex_id,I,E);
	for ( ; I != E; ++I) {
		u32	current = ai().level_graph().value(level_vertex_id,I);
		if (!ai().level_graph().valid_vertex_id(current))
			continue;

		if (restriction->inside(current,!out_restriction) != out_restriction)
			continue;

		return	(true);
	}
	return		(false);
}

template <typename T>
IC	u32	CSpaceRestrictionBridge::accessible_nearest	(T &restriction, const Fvector &position, Fvector &result, bool out_restriction)
{
#pragma todo("Dima to Dima : _Warning : this place can be optimized in case of a slowdown")
	VERIFY							(initialized());

	float							min_dist_sqr = flt_max;
	u32								selected = u32(-1);
	xr_vector<u32>::const_iterator	I = restriction->border().begin();
	xr_vector<u32>::const_iterator	E = restriction->border().end();
	for ( ; I != E; ++I) {
		float	distance_sqr = ai().level_graph().vertex_position(*I).distance_to_sqr(position);
		if (distance_sqr < min_dist_sqr) {
			if (!accessible_neighbours(restriction,*I,out_restriction))
				continue;
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
			// if (out_restriction)
			//		check if node is completely inside
			// else
			//		check if node is completely outside
			if (restriction->inside(current,!out_restriction) != out_restriction)
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
		float		offset = ai().level_graph().header().cell_size()*.5f - EPS;
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

			VERIFY	(restriction->inside(current) == out_restriction);
			float	distance_sqr = current.distance_to(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				result = current;
				found = true;
			}
		}
		VERIFY	(found);
	}
	VERIFY		(ai().level_graph().valid_vertex_id(selected));
	
	return		(selected);
}
