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
	init				();
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
IC	void CGameLocationSelector::init			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::reinit			(const CGameGraph *graph)
{
	inherited::reinit				(graph);
	CAI_ObjectLocation::reinit		();
	
	m_selection_type				= eSelectionTypeRandomBranching;
	m_location_manager				= dynamic_cast<CLocationManager*>(this);
	VERIFY							(m_location_manager);
	m_time_to_change				= 0;
	if (graph)
		graph->set_invalid_vertex	(m_previous_vertex_id);
	else
		m_previous_vertex_id		= ALife::_GRAPH_ID(-1);
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::select_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id)
{
	switch (m_selection_type) {
		case eSelectionTypeMask : {
			if (used())
				perform_search	(start_vertex_id);
			else
				m_failed		= false;
			break;
		}
		case eSelectionTypeRandomBranching : {
			if (m_graph)
				select_random_location(start_vertex_id,dest_vertex_id);
			m_failed			= m_failed && (start_vertex_id == dest_vertex_id);
			break;
		}
		default :				NODEFAULT;
	}
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
	int							iPointCount	= (int)m_location_manager->vertex_types().size();
	int							iBranches	= 0;
	for ( ; i != e; ++i)
		for (int j=0; j<iPointCount; ++j)
			if ((m_graph->vertex((*i).vertex_id())->level_id() == ai().level_graph().level_id()) && m_graph->mask(m_location_manager->vertex_types()[j].tMask,m_graph->vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_previous_vertex_id)) {
				if (!accessible((*i).vertex_id()))
					continue;
				++iBranches;
				if (k < 0)
					k = j;
			}
	if (!iBranches) {
		dest_vertex_id			= m_previous_vertex_id;
		if (m_graph->valid_vertex_id(m_previous_vertex_id) && (m_previous_vertex_id != start_vertex_id)) {
			m_graph->begin		(start_vertex_id,i,e);
			for ( ; i != e; ++i)
				for (int j=0; j<iPointCount; ++j)
					if ((m_graph->vertex((*i).vertex_id())->level_id() == ai().level_graph().level_id()) && m_graph->mask(m_location_manager->vertex_types()[j].tMask,m_graph->vertex((*i).vertex_id())->vertex_type())) {
						if (!accessible((*i).vertex_id()))
							continue;
						dest_vertex_id		= (*i).vertex_id();
						m_time_to_change	= Level().timeServer() + ::Random.randI(m_location_manager->vertex_types()[j].dwMinTime,m_location_manager->vertex_types()[j].dwMaxTime);
						++iBranches;
						break;
					}
			VERIFY				(iBranches);
		}
	}
	else {
		m_graph->begin			(start_vertex_id,i,e);
		int						iChosenBranch = ::Random.randI(0,iBranches);
		iBranches				= 0;
		bool					bOk = false;
		for ( ; i != e; ++i) {
			for (int j=0; j<iPointCount; ++j)
				if ((m_graph->vertex((*i).vertex_id())->level_id() == ai().level_graph().level_id()) && m_graph->mask(m_location_manager->vertex_types()[j].tMask,m_graph->vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_previous_vertex_id)) {
					if (!accessible((*i).vertex_id()))
						continue;
					if (iBranches == iChosenBranch) {
						dest_vertex_id		= (*i).vertex_id();
						m_time_to_change	= Level().timeServer() + ::Random.randI(m_location_manager->vertex_types()[j].dwMinTime,m_location_manager->vertex_types()[j].dwMaxTime);
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
IC	void CGameLocationSelector::selection_type		() const
{
	return				(m_selection_type);
}

TEMPLATE_SPECIALIZATION
IC	bool CGameLocationSelector::actual(const _vertex_id_type start_vertex_id, bool path_completed)
{
	if (m_selection_type != eSelectionTypeRandomBranching)
		return				(inherited::actual(start_vertex_id,path_completed));
	return					(!path_completed);
}

#undef TEMPLATE_SPECIALIZATION
#undef CGameLocationSelector
