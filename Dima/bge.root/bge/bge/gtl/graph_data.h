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
struct helper {
	typedef	_graph_vertex_type<
				_data_type,
				_vertex_id_type,
				helper
			>					vertex_type;

	typedef	_graph_edge_type<
				_edge_weight_type,
				vertex_type
			>					edge_type;

	typedef graph_base<
				_edge_weight_type,
				_vertex_id_type,
				vertex_type,
				edge_type
			> result;
};

template <
	typename _data_type,
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
class _graph : 
	public 
		helper<
			_data_type,
			_edge_weight_type,
			_vertex_id_type,
			_graph_vertex_type,
			_graph_edge_type
		>::result
{
public:
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
	typedef	typename helper<
				_data_type,
				_edge_weight_type,
				_vertex_id_type,
				_graph_vertex_type,
				_graph_edge_type
			>::result			inherited;

public:
	virtual void				save				(IWriter &stream);
	virtual void				load				(IReader &stream);
	IC		void				add_vertex			(const _data_type &data, const _vertex_id_type &vertex_id);
	IC		vertex_type			*create_vertex		(const _vertex_id_type &vertex_id, size_t *edge_count, const _data_type &data);
};

#include "graph_data_inline.h"