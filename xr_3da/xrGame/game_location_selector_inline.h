////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector_inline.h
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type,\
>

#define CGameLocationSelector CBaseLocationSelector<CGameGraph,_VertexEvaluator,_vertex_id_type>

TEMPLATE_SPECIALIZATION
IC	CGameLocationSelector::CBaseLocationSelector	()
{
}

TEMPLATE_SPECIALIZATION
IC	CGameLocationSelector::~CBaseLocationSelector	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::Init				(_Graph *graph = 0)
{
	inherited::Init		(graph);
	m_selection_type	= eSelectionTypeMask || eSelectionTypeRandomBranching;
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::set_selection_type	(const ESelectionType selection_type)
{
	m_selection_type	= selection_type;
}

TEMPLATE_SPECIALIZATION
IC	void CGameLocationSelector::get_selection_type	() const
{
	return				(m_selection_type);
}

#undef TEMPLATE_SPECIALIZATION
