////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_base.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _data_type,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	template <\
		typename,\
		typename,\
		typename\
	> class _graph_vertex_type,\
	template <\
		typename,\
		typename\
	> class _graph_edge_type\
>

#define CAbstractGraph _graph<\
	_data_type,\
	_edge_weight_type,\
	_vertex_id_type,\
	_graph_vertex_type,\
	_graph_edge_type\
>

TEMPLATE_SPECIALIZATION
void CAbstractGraph::save				(IWriter &stream)
{
	inherited::save			(stream);
}

TEMPLATE_SPECIALIZATION
void CAbstractGraph::load				(IReader &stream)
{
	inherited::load			(stream);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::add_vertex		(const _data_type &data, const _vertex_id_type &vertex_id)
{
	inherited::add_vertex	(vertex_id,*this,data);
}

TEMPLATE_SPECIALIZATION
IC	typename CAbstractGraph::vertex_type *CAbstractGraph::create_vertex	(const _vertex_id_type &vertex_id, size_t *edge_count, const _data_type &data)
{
	return					(new vertex_type(vertex_id,data,edge_count));
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractGraph