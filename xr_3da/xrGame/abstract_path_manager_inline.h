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
#include "graph_engine.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::CAbstractPathManager		()
{
	Init					();
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::~CAbstractPathManager	()
{
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::Init				(_Graph *graph)
{
	m_actuality				= false;
	m_intermediate_index	= u32(-1);
	m_path.clear			();
	m_evaluator				= 0;
	m_graph					= graph;
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::build_path	(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id)
{
	VERIFY					(m_graph);
	VERIFY					(m_evaluator);
	if (!actual(start_vertex_id, dest_vertex_id) && m_graph->valid_vertex_id(start_vertex_id) && m_graph->valid_vertex_id(dest_vertex_id)) {
		m_actuality			= ai().graph_engine().search(*m_graph,start_vertex_id,dest_vertex_id,&m_path,*m_evaluator);
		if (m_actuality)
			m_intermediate_index = 0;
		m_actuality			= true;
	}
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::select_intermediate_vertex()
{
	VERIFY					(!m_path.empty());
	m_intermediate_index	= m_path.size() - 1;
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	_vertex_id_type CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::get_intermediate_vertex_id() const
{
	return					(m_intermediate_index);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	u32 CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::get_intermediate_index() const
{
	return					(m_intermediate_index);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::actual(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const
{
	return				(!m_path.empty());
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::completed() const
{
	return				(true);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	bool CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::failed() const
{
	return				(false);
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	void CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::set_evaluator(_VertexEvaluator *evaluator)
{
	m_evaluator			= evaluator;
}

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
IC	const xr_vector<_vertex_id_type> &
CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type>::path	() const
{
	return				(m_path);
}