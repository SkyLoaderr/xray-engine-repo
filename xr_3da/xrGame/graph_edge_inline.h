////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_edge_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph edge class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Data,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	typename _vertex_index_type\
>

#define CSGraphEdge CGraphEdge<_Data,_edge_weight_type,_vertex_id_type,_vertex_index_type>

TEMPLATE_SPECIALIZATION
IC	CSGraphEdge::CGraphEdge												(
	const _edge_weight_type weight, 
	const _vertex_id_type vertex_id, 
	const _vertex_index_type vertex_index
) :
	m_weight		(weight),
	m_vertex_id		(vertex_id),
	m_vertex_index	(vertex_index)
{
}

TEMPLATE_SPECIALIZATION
IC	typename CSGraphEdge::_edge_weight_type CSGraphEdge::weight			() const
{
	return			(m_weight);
}

TEMPLATE_SPECIALIZATION
IC	typename CSGraphEdge::_vertex_id_type CSGraphEdge::vertex_id		() const
{
	return			(m_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	typename CSGraphEdge::_vertex_index_type CSGraphEdge::vertex_index	() const
{
	return			(m_vertex_index);
}

TEMPLATE_SPECIALIZATION
IC	bool CSGraphEdge::operator==										(const _vertex_index_type &index) const
{
	return			(vertex_index() == index);
}

TEMPLATE_SPECIALIZATION
IC	bool CSGraphEdge::operator==										(const CGraphEdge &obj) const
{
	if (weight() != obj.weight())
		return		(false);

	return			(vertex_id() == obj.vertex_id());
}

#undef TEMPLATE_SPECIALIZATION
#undef CSGraphEdge