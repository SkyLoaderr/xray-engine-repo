////////////////////////////////////////////////////////////////////////////
//	Module 		: graph.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_data.h"
#include "graph_no_data.h"
#include "graph_vertex.h"
#include "graph_edge.h"

template <
	typename _data_type = Loki::EmptyType,
	typename _edge_weight_type = float,
	typename _vertex_id_type = u32,
	template <
		typename,
		typename,
		typename
	> class _graph_vertex_type = graph_vertex,
	template <
		typename,
		typename
	> class _graph_edge_type = graph_edge
>
struct graph : 
	public _graph<
		_data_type,
		_edge_weight_type,
		_vertex_id_type,
		_graph_vertex_type,
		_graph_edge_type
	>
{
};
