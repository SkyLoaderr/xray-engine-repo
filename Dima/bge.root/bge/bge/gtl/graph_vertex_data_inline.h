////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _data_type,\
	typename _vertex_id_type,\
	typename _graph_type\
>

#define CSGraphVertex graph_vertex<\
	_data_type,\
	_vertex_id_type,\
	_graph_type\
>

TEMPLATE_SPECIALIZATION
IC	CSGraphVertex::graph_vertex				(const _data_type &data, const _vertex_id_type &vertex_id, size_t *edge_count) :
	inherited		(vertex_id,edge_count)
{
	m_data			= data;
}

TEMPLATE_SPECIALIZATION
IC	CSGraphVertex::~graph_vertex			()
{
	delete_data		(m_data);
}

TEMPLATE_SPECIALIZATION
IC	bool CSGraphVertex::operator==			(const graph_vertex &obj) const
{
	if ((const inherited&)(*this) == (const inherited&)(obj))
		return		(false);

	return			(equal(data(),obj.data()));
}

TEMPLATE_SPECIALIZATION
IC	const _data_type &CSGraphVertex::data	() const
{
	return			(m_data);
}

TEMPLATE_SPECIALIZATION
IC	_data_type &CSGraphVertex::data			()
{
	return			(m_data);
}

TEMPLATE_SPECIALIZATION
IC	void CSGraphVertex::data				(const _data_type &data)
{
	m_data			= data;
}

#undef TEMPLATE_SPECIALIZATION
#undef CSGraphVertex