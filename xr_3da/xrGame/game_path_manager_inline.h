////////////////////////////////////////////////////////////////////////////
//	Module 		: game_path_manager_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type,\
	typename _index_type\
>

#define CGameManagerTemplate CBasePathManager<CGameGraph,_VertexEvaluator,_vertex_id_type,_index_type>

TEMPLATE_SPECIALIZATION
IC	void CGameManagerTemplate::Init()
{
}

TEMPLATE_SPECIALIZATION
IC	void CGameManagerTemplate::reinit(const CGameGraph *graph)
{
	inherited::reinit	(graph);
	CAI_ObjectLocation::reinit();
}

TEMPLATE_SPECIALIZATION
IC	bool CGameManagerTemplate::actual() const
{
	return			(inherited::actual(game_vertex_id(),dest_vertex_id()));
}

#undef TEMPLATE_SPECIALIZATION
#undef CGameManagerTemplate