////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_data.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_base.h"

template <
	typename _data_type,
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
class graph : 
	public 
		graph_base<
			_edge_weight_type,
			_vertex_id_type,
			_graph_vertex_type<
				_data_type,
				_vertex_id_type,
				graph
			>,
			_graph_edge_type<
				_edge_weight_type,
				_graph_vertex_type<
					_data_type,
					_vertex_id_type,
					graph
				>
			>
		>
{
public:
	typedef	_graph_vertex_type<
				_data_type,
				_vertex_id_type,
				graph
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
	virtual void				save				(IWriter &stream);
	virtual void				load				(IReader &stream);
	IC		void				add_vertex			(const _data_type &data, const _vertex_id_type &vertex_id);
	IC		vertex_type			*create_vertex		(const _vertex_id_type &vertex_id, size_t *edge_count);
};

#include "graph_data_inline.h"