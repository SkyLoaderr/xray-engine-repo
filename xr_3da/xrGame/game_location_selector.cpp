////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.cpp
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_location_selector.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type\
>

#define CGameLocationSelector CBaseLocationSelector<CGameGraph,_VertexEvaluator,_vertex_id_type>

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::Init				(_Graph *graph = 0)
{
	// initializing inherited
	inherited::Init					(graph);
	CAI_ObjectLocation::Init		();
	// self-initializing 
	m_selection_type				= eSelectionTypeMask || eSelectionTypeRandomBranching;
	m_time_to_change				= 0;
	if (graph)
		graph->invalid_vertex		(m_previous_vertex_id);
	else
		m_previous_vertex_id		= _vertex_id_type(-1);

	// loading terrain types
	m_vertex_types.clear			();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(!(N % (LOCATION_TYPE_COUNT + 2)) && N);
	ALife::STerrainPlace			l_terrain_location;
	l_terrain_location.tMask.resize	(LOCATION_TYPE_COUNT);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; ++j, ++i)
			l_terrain_location.tMask[j] = _LOCATION_ID(atoi(_GetItem(S,i,I)));
		l_terrain_location.dwMinTime	= atoi(_GetItem(S,i++,I))*1000;
		l_terrain_location.dwMaxTime	= atoi(_GetItem(S,i++,I))*1000;
		m_vertex_types.push_back		(l_terrain_location);
	}
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::select_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id)
{
	if (used()) {
		perform_search	(start_vertex_id);
		if (failed())
			break;
		switch (m_selection_type) {
			case eSelectionTypeMask : {
				break;
			}
			case eSelectionTypeRandomBranching : {
				dest_vertex_id	= m_selected_vertex_id;
				if (dest_vertex_id == start_vertex_id)
					select_random_location(start_vertex_id,dest_vertex_id);
				break;
			}
			default :			NODEFAULT;
		}
	}
	else
		m_failed				= false;
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::select_random_location(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id) const
{
	if (!graph->valid_vertex_id(m_previous_vertex_id))
		m_previous_vertex_id	= start_vertex_id;
	_Graph::const_iterator		i,e;
	VERIFY						(graph);
	graph->begin				(start_vertex_id,i,e);
	int							iPointCount	= (int)m_vertex_types.size();
	int							iBranches	= 0;
	for ( ; i != e; ++i)
		for (int j=0; j<iPointCount; ++j)
			if (ai().game_graph().mask(m_vertex_types[j].tMask,ai().game_graph().vertex((*i).vertex_id()).vertex_type()) && ((*i).vertex_id() != m_previous_vertex_id))
				++iBranches;
	if (!iBranches) {
		VERIFY					(graph->valid_vertex_id(m_previous_vertex_id) && (m_previous_vertex_id != start_vertex_id));
		dest_vertex_id			= m_previous_vertex_id;
		m_time_to_change		= Level().timeServer() + ::Random.randI(m_vertex_types[j].dwMinTime,m_vertex_types[j].dwMaxTime);
	}
	else {
		ai().game_graph().begin	(start_vertex_id,i,e);
		int						iChosenBranch = ::Random.randI(0,iBranches);
		iBranches				= 0;
		for ( ; i != e; ++i) {
			for (int j=0; j<iPointCount; ++j)
				if (ai().game_graph().mask(m_vertex_types[j].tMask,ai().game_graph().vertex((*i).vertex_id()).vertex_type()) && ((*i).vertex_id() != m_tCurGP)) {
					if (iBranches == iChosenBranch) {
						dest_vertex_id		= (*i).vertex_id();
						m_time_to_change	= Level().timeServer() + ::Random.randI(m_vertex_types[j].dwMinTime,m_vertex_types[j].dwMaxTime);
						return;
					}
					++iBranches;
				}
		}
	}
	m_previous_vertex_id		= start_vertex_id;
}

#undef TEMPLATE_SPECIALIZATION
