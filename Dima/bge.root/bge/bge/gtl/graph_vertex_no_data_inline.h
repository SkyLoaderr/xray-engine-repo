////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_no_data_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _vertex_id_type,\
	typename _graph_type\
>

#define CSGraphVertex graph_vertex<\
	Loki::EmptyType,\
	_vertex_id_type,\
	_graph_type\
>

/**
TEMPLATE_SPECIALIZATION
IC	CSGraphVertex::graph_vertex	(const _vertex_id_type &vertex_id, size_t *edge_count) :
	inherited	(vertex_id,edge_count)
{
}
/**/
	
#undef TEMPLATE_SPECIALIZATION
#undef CSGraphVertex