////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_base.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex base class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_broker.h"

template <
	typename _vertex_id_type,
	typename _graph_type
>
class graph_vertex_base {
public:
	typedef _vertex_id_type									_vertex_id_type;
	typedef typename _graph_type::edge_type					_edge_type;
	typedef typename _edge_type::_edge_weight_type			_edge_weight_type;
	typedef xr_vector<_edge_type>							EDGES;
	typedef xr_vector<graph_vertex_base*>					VERTICES;

private:
	_vertex_id_type						m_vertex_id;
	EDGES								m_edges;
	// this container holds vertices, which has edges to us
	// this is needed for the fast vertex removal
	VERTICES							m_vertices;
	// this counter is use for fast edge count computation in graph
	size_t								*m_edge_count;

public:
	IC									graph_vertex_base	(const _vertex_id_type &vertex_id, size_t *edge_count);
	// destructor is not virtual, 
	// since we shouldn't remove vertices,
	// having pointer not to their final class
	IC									~graph_vertex_base	();
	IC		bool						operator==			(const graph_vertex_base &obj) const;
	IC		void						add_edge			(graph_vertex_base *vertex, const _edge_weight_type &edge_weight);
	IC		void						remove_edge			(const _vertex_id_type &vertex_id);
	IC		void						on_edge_addition	(graph_vertex_base *vertex);
	IC		void						on_edge_removal		(const graph_vertex_base *vertex);
	IC		const _vertex_id_type		&vertex_id			() const;
	IC		const EDGES					&edges				() const;
	IC		const _edge_type			*edge				(const _vertex_id_type &vertex_id) const;
	IC		_edge_type					*edge				(const _vertex_id_type &vertex_id);
};

#include "graph_vertex_base_inline.h"