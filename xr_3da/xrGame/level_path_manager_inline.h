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
IC	void CLevelManagerTemplate::Init()
{
}

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

	VERIFY					(ai().level_graph().valid_vertex_id(start_vertex_id) && ai().level_graph().valid_vertex_id(dest_vertex_id));
//	if (ai().level_graph().check_vertex_in_direction(start_vertex_id,ai().level_graph().vertex_position(start_vertex_id),dest_vertex_id)) {
//		m_path.resize			(2);
//		m_path[0]				= start_vertex_id;
//		m_path[1]				= dest_vertex_id;
//		m_failed				= false;
//		m_current_index			= _index_type(0);
//		m_intermediate_index	= _index_type(0);
//		m_actuality				= !failed();
//		return;
//	}
	inherited::build_path		(start_vertex_id,dest_vertex_id);
#ifdef DEBUG
	if (failed()) {
		Msg						("! Cannot build path from \n[%d][%f][%f][%f]\nto\n[%d][%f][%f][%f]",start_vertex_id,VPUSH(ai().level_graph().vertex_position(start_vertex_id)),dest_vertex_id,VPUSH(ai().level_graph().vertex_position(dest_vertex_id)));
	}
#endif
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelManagerTemplate