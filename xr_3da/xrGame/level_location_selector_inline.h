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
IC	void CLevelLocationSelector::before_search	()
{
	if (m_restricted_object)
		m_restricted_object->add_border();
}

TEMPLATE_SPECIALIZATION
IC	void CLevelLocationSelector::after_search	()
{
	if (m_restricted_object)
		m_restricted_object->remove_border();
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelLocationSelector
