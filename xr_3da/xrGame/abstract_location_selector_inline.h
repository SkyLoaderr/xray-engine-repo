////////////////////////////////////////////////////////////////////////////
//	Module 		: location_selector_abstract_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract location selector inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::CAbstractLocationSelector	()
{
	m_selector_failed		= true;
	m_selected_vertex_id	= _vertex_id_type(-1);
	m_selector_evaluator	= 0;
	m_last_query_time		= 0;
	m_query_interval		= 0;
	m_graph					= 0;
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::~CAbstractLocationSelector	()
{
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::perform_search		(const _vertex_id_type vertex_id)
{
	VERIFY				(m_selector_evaluator && m_graph);
	ai().graph_search_engine().build_path(m_graph,vertex_id,vertex_id,0,m_selector_evaluator);
	m_selector_failed	= 
		!graph->valid_vertex_id(m_selector_evaluator->m_vertex_id) || 
		(m_selector_evaluator->m_vertex_id == m_selected_vertex_id);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::select_location	(const ALife::_GRAPH_ID vertex_id)
{
	if (!m_selector_failed) {
		perform_search	(m_start_vertex_id = vertex_id);
		return;
	}
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::location_selection_actual()
{
	perform_search		(m_start_vertex_id);
	return				(m_selector_failed);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	_vertex_id_type CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::selected_vertex_id() const
{
	VERIFY					(graph->valid_vertex_id(m_selected_vertex_id));
	return					(m_selected_vertex_id);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::set_selector_evaluator	(_VertexEvaluator *vertex_evaluator)
{
	m_selector_evaluator	= vertex_evaluator;
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::selector_failed() const
{
	return					(m_selector_failed);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>::set_query_interval(const u32 query_interval)
{
	m_query_interval		= query_interval;
}
