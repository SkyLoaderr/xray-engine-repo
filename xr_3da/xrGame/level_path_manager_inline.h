////////////////////////////////////////////////////////////////////////////
//	Module 		: level_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Level path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type,\
	typename _index_type\
>

#define CLevelManagerTemplate CBasePathManager<CLevelGraph,_VertexEvaluator,_vertex_id_type,_index_type>

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::reinit(const CLevelGraph *graph)
{
	inherited::reinit	(graph);
	CAI_ObjectLocation::reinit();
}

TEMPLATE_SPECIALIZATION
IC	bool CLevelManagerTemplate::actual() const
{
	return		(inherited::actual(level_vertex_id(),dest_vertex_id()));
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::build_path	(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id, bool use_selector_path)
{
	if (use_selector_path) { 
		m_failed				= false;
		m_actuality				= true;
		return;
	}

	VERIFY						(ai().level_graph().valid_vertex_id(start_vertex_id) && ai().level_graph().valid_vertex_id(dest_vertex_id));
	
	inherited::build_path		(start_vertex_id,dest_vertex_id);

#ifdef DEBUG
	if (failed()) {
		Msg						("! Cannot build path from \n[%d][%f][%f][%f]\nto\n[%d][%f][%f][%f]",start_vertex_id,VPUSH(ai().level_graph().vertex_position(start_vertex_id)),dest_vertex_id,VPUSH(ai().level_graph().vertex_position(dest_vertex_id)));
	}
#endif
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::before_search			(const _vertex_id_type start_vertex_id, const _vertex_id_type dest_vertex_id)
{
	if (m_restricted_object) {
		m_restricted_object->add_border(start_vertex_id,dest_vertex_id);
		VERIFY					(!m_restricted_object->applied() || ai().level_graph().is_accessible(start_vertex_id));
		VERIFY					(!m_restricted_object->applied() || ai().level_graph().is_accessible(dest_vertex_id));
	}
}

TEMPLATE_SPECIALIZATION
IC	void CLevelManagerTemplate::after_search			()
{
	if (m_restricted_object)
		m_restricted_object->remove_border();
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelManagerTemplate