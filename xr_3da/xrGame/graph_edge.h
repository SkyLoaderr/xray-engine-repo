////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_edge.h
//	Created 	: 14.01.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph edge class
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _1, typename _2, typename _3, typename _4> class CGraphVertex;

template <
	typename _Data,
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _vertex_index_type
>
class CGraphEdge {
public:
	typedef _Data				_Data;
	typedef _edge_weight_type	_edge_weight_type;
	typedef _vertex_id_type		_vertex_id_type;
	typedef _vertex_index_type	_vertex_index_type;
	typedef CGraphEdge<
		_Data,
		_edge_weight_type,
		_vertex_id_type,
		_vertex_index_type
	>							CSGraphEdge;

private:
	friend class CGraphVertex<_Data,_edge_weight_type,_vertex_id_type,_vertex_index_type>;

private:
	_edge_weight_type			m_weight;
	_vertex_id_type				m_vertex_id;
	_vertex_index_type			m_vertex_index;

public:

	IC							CGraphEdge		(const _edge_weight_type weight, const _vertex_id_type vertex_id, const _vertex_index_type vertex_index);
	IC	_edge_weight_type		weight			() const;
	IC	_vertex_id_type			vertex_id		() const;
	IC	_vertex_index_type		vertex_index	() const;
	IC	bool					operator==		(const _vertex_index_type &) const;

	template <typename _Stream>
	IC	void					save			(_Stream &stream) const;

	template <typename _Stream>
	IC	void					load			(_Stream &stream);
};

#include "graph_edge_inline.h"