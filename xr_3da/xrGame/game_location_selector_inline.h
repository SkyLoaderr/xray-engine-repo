////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type\
>

#define CGameLocationSelector CBaseLocationSelector<CGameGraph,_VertexEvaluator,_vertex_id_type>

TEMPLATE_SPECIALIZATION
IC	CGameLocationSelector::CBaseLocationSelector	()
{
}

TEMPLATE_SPECIALIZATION
IC	CGameLocationSelector::~CBaseLocationSelector	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::set_selection_type	(const ESelectionType selection_type)
{
	m_selection_type	= selection_type;
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::Init			(const CGameGraph *graph)
{
	// initializing inherited
	inherited::Init					(graph);
	CAI_ObjectLocation::Init		();
	// self-initializing 
	m_selection_type				= eSelectionTypeRandomBranching;
	m_time_to_change				= 0;
	if (graph)
		graph->set_invalid_vertex	(m_previous_vertex_id);
	else
		m_previous_vertex_id		= ALife::_GRAPH_ID(-1);
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::select_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id)
{
	if (used()) {
		perform_search	(start_vertex_id);
		if (failed())
			return;
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
IC	void CGameLocationSelector::select_random_location(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id)
{
	if (!m_graph->valid_vertex_id(m_previous_vertex_id))
		m_previous_vertex_id	= ALife::_GRAPH_ID(start_vertex_id);
	_Graph::const_iterator		i,e;
	int							k = -1;
	VERIFY						(m_graph);
	m_graph->begin				(start_vertex_id,i,e);
	int							iPointCount	= (int)m_vertex_types.size();
	int							iBranches	= 0;
	for ( ; i != e; ++i)
		for (int j=0; j<iPointCount; ++j)
			if (m_graph->mask(m_vertex_types[j].tMask,m_graph->vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_previous_vertex_id)) {
				++iBranches;
				if (k < 0)
					k = j;
			}
	if (!iBranches) {
		VERIFY					(m_graph->valid_vertex_id(m_previous_vertex_id) && (m_previous_vertex_id != start_vertex_id));
		dest_vertex_id			= m_previous_vertex_id;
		m_time_to_change		= Level().timeServer() + ::Random.randI(m_vertex_types[k].dwMinTime,m_vertex_types[k].dwMaxTime);
	}
	else {
		m_graph->begin			(start_vertex_id,i,e);
		int						iChosenBranch = ::Random.randI(0,iBranches);
		iBranches				= 0;
		bool					bOk = false;
		for ( ; i != e; ++i) {
			for (int j=0; j<iPointCount; ++j)
				if (m_graph->mask(m_vertex_types[j].tMask,m_graph->vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_previous_vertex_id)) {
					if (iBranches == iChosenBranch) {
						dest_vertex_id		= (*i).vertex_id();
						m_time_to_change	= Level().timeServer() + ::Random.randI(m_vertex_types[j].dwMinTime,m_vertex_types[j].dwMaxTime);
						bOk = true;
						break;
					}
					++iBranches;
				}
			if (bOk)
				break;
		}
	}
	m_previous_vertex_id		= ALife::_GRAPH_ID(start_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::get_selection_type	() const
{
	return				(m_selection_type);
}

#undef TEMPLATE_SPECIALIZATION
#undef CGameLocationSelector
