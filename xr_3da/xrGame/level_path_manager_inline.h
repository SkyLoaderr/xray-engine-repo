////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "profiler.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type,\
	typename _index_type\
>

#define CLevelManagerTemplate CBasePathManager<CLevelNavigationGraph,_VertexEvaluator,_vertex_id_type,_index_type>

TEMPLATE_SPECIALIZATION
IC	CLevelManagerTemplate::CBasePathManager	(CRestrictedObject *object) :
	inherited(object)
{
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::reinit(const CLevelNavigationGraph *graph)
{
	inherited::reinit			(graph);
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelManagerTemplate::actual() const
{
	return		(inherited::actual(m_object->object().ai_location().level_vertex_id(),dest_vertex_id()));
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::build_path	(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id, bool use_selector_path)
{
	START_PROFILE("AI/Build Path/Level Path");
	
	if (use_selector_path) { 
		m_failed				= false;
		m_actuality				= true;
		return;
	}

	VERIFY						(ai().level_graph().valid_vertex_id(start_vertex_id) && ai().level_graph().valid_vertex_id(dest_vertex_id));
	
	inherited::build_path		(start_vertex_id,dest_vertex_id);

#ifdef DEBUG
	if (failed()) {
		Msg						("! NPC %s couldn't build path from \n! [%d][%f][%f][%f]\n! to\n! [%d][%f][%f][%f]",*m_object->object().cName(),start_vertex_id,VPUSH(ai().level_graph().vertex_position(start_vertex_id)),dest_vertex_id,VPUSH(ai().level_graph().vertex_position(dest_vertex_id)));
	}
#endif

	STOP_PROFILE;
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::before_search			(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id)
{
	if (m_object) {
		m_object->add_border	(start_vertex_id,dest_vertex_id);
		VERIFY					(!m_object->applied() || ai().level_graph().is_accessible(start_vertex_id));
		VERIFY					(!m_object->applied() || ai().level_graph().is_accessible(dest_vertex_id));
	}
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::after_search			()
{
	if (m_object)
		m_object->remove_border();
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelManagerTemplate::check_vertex			(const _vertex_id_type vertex_id) const
{
	return						(inherited::check_vertex(vertex_id) && (!m_object || object().accessible(vertex_id)));
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelManagerTemplate