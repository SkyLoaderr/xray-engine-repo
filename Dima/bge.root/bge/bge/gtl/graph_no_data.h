////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_no_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_base.h"
#include "loki/emptytype.h"

template <
	typename _edge_weight_type,
	typename _vertex_id_type,
	template <
		typename,
		typename,
		typename
	> class _graph_vertex_type,
	template <
		typename,
		typename
	> class _graph_edge_type
>
class 
	_graph<
		Loki::EmptyType,
		_edge_weight_type,
		_vertex_id_type,
		_graph_vertex_type,
		_graph_edge_type
	> : 
	public 
		graph_base<
			_edge_weight_type,
			_vertex_id_type,
			_graph_vertex_type<
				Loki::EmptyType,
				_vertex_id_type,
				_graph
			>,
			_graph_edge_type<
				_edge_weight_type,
				_graph_vertex_type<
					Loki::EmptyType,
					_vertex_id_type,
					_graph
				>
			>
		>
{
public:
	typedef Loki::EmptyType _data_type;

	typedef	_graph_vertex_type<
				_data_type,
				_vertex_id_type,
				_graph
			>					vertex_type;

	typedef	_graph_edge_type<
				_edge_weight_type,
				vertex_type
			>					edge_type;

private:
	typedef graph_base<
				_edge_weight_type,
				_vertex_id_type,
				vertex_type,
				edge_type
			>					inherited;

public:
	IC		void				add_vertex			(const _vertex_id_type &vertex_id);
	IC		vertex_type			*create_vertex		(const _vertex_id_type &vertex_id, size_t *edge_count, bool);
};

#include "graph_no_data_inline.h"