////////////////////////////////////////////////////////////////////////////
//	Module 		: location_selector_abstract_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract location selector inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Graph,\
	typename _VertexEvaluator,\
	typename _vertex_id_type\
>

#define CSelectorTemplate CAbstractLocationSelector<_Graph,_VertexEvaluator,_vertex_id_type>

TEMPLATE_SPECIALIZATION
IC	CSelectorTemplate::CAbstractLocationSelector	()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
IC	CSelectorTemplate::~CAbstractLocationSelector	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CSelectorTemplate::Init					(const _Graph *graph)
{
	m_failed				= true;
	m_selected_vertex_id	= _vertex_id_type(-1);
	m_evaluator				= 0;
	m_last_query_time		= 0;
	m_query_interval		= 0;
	m_graph					= graph;
}

TEMPLATE_SPECIALIZATION
IC	_vertex_id_type CSelectorTemplate::get_selected_vertex_id() const
{
	VERIFY					(m_graph->valid_vertex_id(m_selected_vertex_id));
	return					(m_selected_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	void CSelectorTemplate::set_evaluator	(_VertexEvaluator *evaluator)
{
	m_evaluator				= evaluator;
}

TEMPLATE_SPECIALIZATION
IC	void CSelectorTemplate::set_query_interval(const u32 query_interval)
{
	m_query_interval		= query_interval;
}

TEMPLATE_SPECIALIZATION
IC	bool CSelectorTemplate::actual(const _vertex_id_type start_vertex_id)
{
	if (!used())
		return				(true);
	perform_search			(start_vertex_id);
	return					(failed());
}

TEMPLATE_SPECIALIZATION
IC	bool CSelectorTemplate::failed() const
{
	return					(m_failed);
}

TEMPLATE_SPECIALIZATION
IC	bool CSelectorTemplate::used() const
{
	return					(m_evaluator && m_graph);
}

TEMPLATE_SPECIALIZATION
IC	void CSelectorTemplate::select_location	(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id)
{
	if (used()) {
		perform_search		(start_vertex_id);
		if (!failed())
			dest_vertex_id	= m_selected_vertex_id;
	}
	else
		m_failed			= false;
}

TEMPLATE_SPECIALIZATION
IC	void CSelectorTemplate::perform_search		(const _vertex_id_type vertex_id)
{
	VERIFY						(m_evaluator && m_graph);
	ai().graph_engine().search	(*m_graph,vertex_id,vertex_id,0,*m_evaluator);
	m_failed	= 
		!m_graph->valid_vertex_id(m_evaluator->selected_vertex_id()) || 
		(m_evaluator->selected_vertex_id() == m_selected_vertex_id);
	if (!failed())
		m_selected_vertex_id	= m_evaluator->selected_vertex_id();
}

#undef CSelectorTemplate
#undef TEMPLATE_SPECIALIZATION
