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

#define TEMPLATE_SPECIALIZATION template <\
	typename _Graph,\
	typename _VertexEvaluator,\
	typename _vertex_id_type,\
	typename _index_type\
>

#define CPathManagerTemplate CAbstractPathManager<_Graph,_VertexEvaluator,_vertex_id_type,_index_type>

TEMPLATE_SPECIALIZATION
IC	CPathManagerTemplate::CAbstractPathManager		()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
IC	CPathManagerTemplate::~CAbstractPathManager	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CPathManagerTemplate::Init				(const _Graph *graph)
{
	m_actuality				= false;
	m_failed				= false;
	m_evaluator				= 0;
	m_graph					= graph;
	m_current_index			= _index_type(-1);
	m_intermediate_index	= _index_type(-1);
	m_path.clear			();
}

TEMPLATE_SPECIALIZATION
IC	void CPathManagerTemplate::build_path	(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id)
{
	VERIFY					(m_graph && m_evaluator && m_graph->valid_vertex_id(start_vertex_id) && m_graph->valid_vertex_id(dest_vertex_id));
	m_failed				= !ai().graph_engine().search(*m_graph,start_vertex_id,dest_vertex_id,&m_path,*m_evaluator);
	if (failed()) {
		m_current_index		= _index_type(-1);
		m_intermediate_index= _index_type(-1);
	}
	else {
		m_current_index		= _index_type(0);
		m_intermediate_index= _index_type(0);
	}
	m_actuality				= !failed();
}

TEMPLATE_SPECIALIZATION
IC	void CPathManagerTemplate::select_intermediate_vertex()
{
	VERIFY					(!failed() && !m_path.empty());
	m_intermediate_index	= m_path.size() - 1;
}

TEMPLATE_SPECIALIZATION
IC	_vertex_id_type CPathManagerTemplate::intermediate_vertex_id() const
{
	VERIFY					(m_intermediate_index < m_path.size());
	return					(m_path[intermediate_index()]);
}

TEMPLATE_SPECIALIZATION
IC	u32 CPathManagerTemplate::intermediate_index() const
{
	return					(m_intermediate_index);
}

TEMPLATE_SPECIALIZATION
IC	bool CPathManagerTemplate::actual(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id) const
{
	return					(m_actuality);
}

TEMPLATE_SPECIALIZATION
IC	bool CPathManagerTemplate::completed() const
{
	return					(m_intermediate_index == m_path.size() - 1);
}

TEMPLATE_SPECIALIZATION
IC	bool CPathManagerTemplate::failed() const
{
	return					(m_failed);
}

TEMPLATE_SPECIALIZATION
IC	void CPathManagerTemplate::set_evaluator(_VertexEvaluator *evaluator)
{
	if ((evaluator != m_evaluator) || !m_evaluator->actual())
		m_actuality			= false;
	m_evaluator				= evaluator;
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<_vertex_id_type> &CPathManagerTemplate::path	() const
{
	return					(m_path);
}

TEMPLATE_SPECIALIZATION
IC	_vertex_id_type CPathManagerTemplate::dest_vertex_id() const
{
	return					(m_dest_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	void CPathManagerTemplate::set_dest_vertex(const _vertex_id_type vertex_id)
{
	m_actuality				= m_actuality && (dest_vertex_id() == vertex_id);
	m_dest_vertex_id		= vertex_id;
}

#undef TEMPLATE_SPECIALIZATION
