////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction.h"
#include "space_restriction_base.h"
#include "space_restriction_bridge.h"
#include "space_restriction_manager.h"
#include "ai_space.h"
#include "level_graph.h"

const float dependent_distance = 100.f;

template <bool a>
struct CMergeInOutPredicate {
	SpaceRestrictionHolder::CBaseRestrictionPtr m_out;
	SpaceRestrictionHolder::CBaseRestrictionPtr m_in;
	
	IC			CMergeInOutPredicate(SpaceRestrictionHolder::CBaseRestrictionPtr out, SpaceRestrictionHolder::CBaseRestrictionPtr in)
	{
		m_out						= out;
		m_in						= in;
	}

	IC	bool	operator()			(u32 level_vertex_id) const
	{
		if (!m_out || !m_in)
			return					(false);
		return						(a ? m_in->inside(level_vertex_id) : !m_out->inside(level_vertex_id) || m_in->inside(level_vertex_id));
	}
};

struct CRemoveMergedFreeInRestrictions {
	CSpaceRestriction::RESTRICTIONS	*m_restrictions;

	IC			CRemoveMergedFreeInRestrictions	(CSpaceRestriction::RESTRICTIONS &restrictions)
	{
		m_restrictions				= &restrictions;
	}

	IC	bool	operator()						(const CSpaceRestriction::CFreeInRestriction &free_in_restriction) const
	{
		return						(std::find(m_restrictions->begin(),m_restrictions->end(), free_in_restriction.m_restriction) != m_restrictions->end());
	}
};

IC	bool CSpaceRestriction::intersects			(SpaceRestrictionHolder::CBaseRestrictionPtr bridge0, SpaceRestrictionHolder::CBaseRestrictionPtr bridge1)
{
	xr_vector<u32>::const_iterator	I = bridge1->border().begin();
	xr_vector<u32>::const_iterator	E = bridge1->border().end();
	for ( ; I != E; ++I)
		if (bridge0->inside(*I))
			return					(true);
	return							(false);
}

IC	bool CSpaceRestriction::intersects			(SpaceRestrictionHolder::CBaseRestrictionPtr bridge)
{
	if (!m_out_space_restriction)
		return						(false);

	return							(intersects(m_out_space_restriction,bridge));
}

void CSpaceRestriction::merge_in_out_restrictions	()
{
	RESTRICTIONS					intersected_in_restrictions;
	xr_vector<u32>					temp_border;

	string256						temp;
	for (u32 i=0, n=_GetItemCount(*m_in_restrictions); i<n ;++i) {
		SpaceRestrictionHolder::CBaseRestrictionPtr bridge = m_space_restriction_manager->restriction(ref_str(_GetItem(*m_in_restrictions,i,temp)));
		if (!intersects(bridge))
			m_free_in_restrictions.push_back		(CFreeInRestriction(bridge,false));
		else
			intersected_in_restrictions.push_back	(bridge);
	}

	if (m_out_space_restriction)
		m_border					= m_out_space_restriction->border();

	{
		RESTRICTIONS::iterator		I = intersected_in_restrictions.begin();
		RESTRICTIONS::iterator		E = intersected_in_restrictions.end();
		for ( ; I != E; ++I)
			temp_border.insert		(temp_border.end(),(*I)->border().begin(),(*I)->border().end());
	}
	
	xr_vector<u32>::iterator		I;

	I								= remove_if(m_border.begin(),m_border.end(),CMergeInOutPredicate<true>(m_out_space_restriction,m_in_space_restriction));
	m_border.erase					(I,m_border.end());

	I								= remove_if(temp_border.begin(),temp_border.end(),CMergeInOutPredicate<false>(m_out_space_restriction,m_in_space_restriction));
	temp_border.erase				(I,temp_border.end());

	m_border.insert					(m_border.end(),temp_border.begin(),temp_border.end());
	
	std::sort						(m_border.begin(),m_border.end());
	I								= unique(m_border.begin(),m_border.end());
	m_border.erase					(I,m_border.end());
}


CSpaceRestriction::CBaseRestrictionPtr CSpaceRestriction::merge	(CBaseRestrictionPtr bridge, const RESTRICTIONS &temp_restrictions) const
{
	string256						S;
	ref_str							temp = bridge->name();
	RESTRICTIONS::const_iterator	I = temp_restrictions.begin();
	RESTRICTIONS::const_iterator	E = temp_restrictions.end();
	for ( ; I != E; ++I)
		temp						= strconcat(S,*temp,",",*(*I)->name());
	return							(m_space_restriction_manager->restriction(temp));
}

void CSpaceRestriction::merge_free_in_retrictions	()
{
	RESTRICTIONS					temp_restrictions;
	for (bool ok = false; !ok; ) {
		ok							= true;
		temp_restrictions.clear		();
		FREE_IN_RESTRICTIONS::iterator	I = m_free_in_restrictions.begin(), J;
		FREE_IN_RESTRICTIONS::iterator	E = m_free_in_restrictions.end();
		for ( ; I != E; ++I) {
			for (J = I + 1; J != E; ++J)
				if (intersects((*I).m_restriction,(*J).m_restriction))
					temp_restrictions.push_back	((*J).m_restriction);

			if (!temp_restrictions.empty()) {
				J					= remove_if(m_free_in_restrictions.begin(),m_free_in_restrictions.end(),CRemoveMergedFreeInRestrictions(temp_restrictions));
				m_free_in_restrictions.erase	(J,m_free_in_restrictions.end());
				(*I).m_restriction	= merge((*I).m_restriction,temp_restrictions);
				ok					= false;
				break;
			}
		}
	}
}

void CSpaceRestriction::initialize					()
{
	VERIFY							(!m_initialized);
	m_out_space_restriction			= m_space_restriction_manager->restriction(m_out_restrictions);
	m_in_space_restriction			= m_space_restriction_manager->restriction(m_in_restrictions);
	
	if (!m_out_space_restriction && !m_in_space_restriction) {
		m_initialized				= true;
		return;
	}

	if (m_out_space_restriction && !m_out_space_restriction->initialized())
		m_out_space_restriction->initialize();

	if (m_in_space_restriction && !m_in_space_restriction->initialized())
		m_in_space_restriction->initialize();

	if ((m_out_space_restriction && !m_out_space_restriction->initialized()) || (m_in_space_restriction && !m_in_space_restriction->initialized()))
		return;

	merge_in_out_restrictions		();
	merge_free_in_retrictions		();

	m_initialized					= true;
}

void CSpaceRestriction::remove_border			()
{
	if (!initialized())
		return;
	
	VERIFY							(m_applied);
	
	m_applied						= false;
	
	ai().level_graph().clear_mask	(border());

	FREE_IN_RESTRICTIONS::iterator	I = m_free_in_restrictions.begin();
	FREE_IN_RESTRICTIONS::iterator	E = m_free_in_restrictions.end();
	for ( ; I != E; ++I)
		if ((*I).m_enabled) {
			VERIFY							((*I).m_restriction);
			(*I).m_enabled					= false;
			ai().level_graph().clear_mask	((*I).m_restriction->border());
		}
}

IC	bool CSpaceRestriction::accessible_neighbours	(u32 level_vertex_id)
{
	CLevelGraph::const_iterator	i,e;
	ai().level_graph().begin(level_vertex_id,i,e);
	for ( ; i != e; ++i) {
		u32					neighbour_vertex_id = ai().level_graph().value(level_vertex_id,i);
		if (!ai().level_graph().valid_vertex_id(neighbour_vertex_id) || !accessible(neighbour_vertex_id))
			return			(false);
	}
	return					(true);
}

u32 CSpaceRestriction::accessible_nearest		(CBaseRestrictionPtr restriction, const Fvector &position)
{
	u32								result = u32(-1);
	float							min_dist_sqr = flt_max;
	xr_vector<u32>::const_iterator	I = restriction->border().begin();
	xr_vector<u32>::const_iterator	E = restriction->border().end();
	for ( ; I != E; ++I) {
		float						cur_dist_sqr = ai().level_graph().vertex_position(*I).distance_to_sqr(position);
		if (cur_dist_sqr < min_dist_sqr) {
			bool					found = false;
			CLevelGraph::const_iterator	i,e;
			ai().level_graph().begin(*I,i,e);
			for ( ; i != e; ++i) {
				u32					level_vertex_id = ai().level_graph().value(*I,i);
				if (ai().level_graph().valid_vertex_id(level_vertex_id) && accessible_neighbours(level_vertex_id)) {
					found			= true;
					result			= level_vertex_id;
					break;
				}
			}
			if (!found)
				continue;
			min_dist_sqr			= cur_dist_sqr;
		}
	}
	VERIFY							(ai().level_graph().valid_vertex_id(result));
	return							(result);
}

u32	CSpaceRestriction::accessible_nearest		(const Fvector &position, Fvector &result)
{
	if (m_out_space_restriction)
		return						(m_out_space_restriction->accessible_nearest(position,result));

	u32								vertex_result = u32(-1);
	if (m_in_space_restriction && m_in_space_restriction->inside(position))
		vertex_result				= accessible_nearest(m_in_space_restriction,position);
	else {
		FREE_IN_RESTRICTIONS::const_iterator	I = m_free_in_restrictions.begin();
		FREE_IN_RESTRICTIONS::const_iterator	E = m_free_in_restrictions.end();
		for ( ; I != E; ++I)
			if ((*I).m_restriction->inside(position)) {
				vertex_result		= accessible_nearest((*I).m_restriction,position);
				break;
			}
	}

	VERIFY							(ai().level_graph().valid_vertex_id(vertex_result));
	result							= ai().level_graph().vertex_position(vertex_result);
	return							(vertex_result);
}

bool CSpaceRestriction::affect					(SpaceRestrictionHolder::CBaseRestrictionPtr bridge, const Fvector &start_position, float radius) const
{
	if (bridge->inside(start_position))
		return						(false);
	Fvector							position;
	bridge->accessible_nearest		(start_position,position);
	return							(start_position.distance_to(position) <= radius + dependent_distance);
}

bool CSpaceRestriction::affect					(SpaceRestrictionHolder::CBaseRestrictionPtr bridge, u32 start_vertex_id, float radius) const
{
	return							(affect(bridge,ai().level_graph().vertex_position(start_vertex_id),radius));
}

bool CSpaceRestriction::affect					(SpaceRestrictionHolder::CBaseRestrictionPtr bridge, const Fvector &start_position, const Fvector &dest_position) const
{
	return							(affect(bridge,start_position,0.f) || affect(bridge,dest_position,0.f));
}

bool CSpaceRestriction::affect					(SpaceRestrictionHolder::CBaseRestrictionPtr bridge, u32 start_vertex_id, u32 dest_vertex_id) const
{
	return							(affect(bridge,start_vertex_id,0.f) || affect(bridge,dest_vertex_id,0.f));
}
