////////////////////////////////////////////////////////////////////////////
//	Module 		: abstract_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "abstract_path_manager.h"
#include "ai_space.h"
#include "graph_search_engine.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::CAbstractPathManager		()
{
	init					();
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::~CAbstractPathManager		()
{
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::init			()
{
	m_path_actuality	= false;
	m_cur_node_index	= u32(-1);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::build_path	(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id)
{
	if (!path_actual(start_vertex_id, dest_vertex_id) && m_graph->valid_vertex_id(start_vertex_id) && m_graph->valid_vertex_id(dest_vertex_id)) {
		m_path_actuality	= ai().graph_search_engine().build_path(*m_graph,start_vertex_id,dest_vertex_id,&m_path,m_path_evaluator);
		if (m_path_actuality)
			m_cur_node_index	= 0;
		m_path_actuality	= true;
	}
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::select_intermediate_vertex()
{
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::path_actual(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const
{
	return				(true);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::path_completed() const
{
	return				(true);
}