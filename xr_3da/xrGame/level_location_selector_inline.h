////////////////////////////////////////////////////////////////////////////
//	Module 		: level_location_selector_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level location selector inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type\
>

#define CLevelLocationSelector CBaseLocationSelector<CLevelGraph,_VertexEvaluator,_vertex_id_type>


TEMPLATE_SPECIALIZATION
IC	void CLevelLocationSelector::perform_search		(const _vertex_id_type vertex_id)
{
	VERIFY						(m_evaluator && m_graph);
	
	// OLES	
	// Msg("PRE :: Time =[%u] My Node = [%u] Best Node = [%u]", Level().timeServer(), m_evaluator->m_dwStartNode, m_evaluator->m_dwBestNode);

	m_evaluator->m_path			= m_path;
	ai().graph_engine().search	(*m_graph,vertex_id,vertex_id,0,*m_evaluator);
	
	// OLES	
	// Msg("POST :: Time =[%u] My Node = [%u] Best Node = [%u]", Level().timeServer(), m_evaluator->m_dwStartNode, m_evaluator->m_dwBestNode);
	
	m_failed	= 
		!m_graph->valid_vertex_id(m_evaluator->selected_vertex_id()) || 
		(m_evaluator->selected_vertex_id() == m_selected_vertex_id);
	
	// OLES	
	/* 
	if (m_failed && (m_evaluator->selected_vertex_id() == m_selected_vertex_id)) {
		Msg("Failed ___ already selected");
	}
	*/

	if (!failed())
		m_selected_vertex_id	= m_evaluator->selected_vertex_id();
}



#undef TEMPLATE_SPECIALIZATION
#undef CLevelLocationSelector
