////////////////////////////////////////////////////////////////////////////
//	Module 		: graph.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph class template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_vertex.h"
#include "graph_edge.h"
#include "object_broker.h"

class IReader;
class IWriter;

// TODO:
//		1. add container_type for holding graph vertices
//		2. add container_type for holding graph edges in graph_vertex
//		3. add container_type for holding back references in graph_vertex

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
class graph : public IPureSerializeObject<IReader,IWriter> {
public:
	typedef _graph_vertex_type<
		_data_type,
		_vertex_id_type,
		graph
	>												vertex_type;

	typedef _graph_edge_type<
		_edge_weight_type,
		vertex_type
	>												edge_type;

private:
	struct vertex_predicate {
		IC	const _vertex_id_type &operator()	(const vertex_type *vertex) const
		{
			return				(vertex->vertex_id());
		}
	};

public:
	typedef xr_set<vertex_type*,vertex_predicate>	VERTICES;
	typedef typename vertex_type::EDGES				EDGES;

public:
	typedef typename VERTICES::const_iterator		const_vertex_iterator;
	typedef typename VERTICES::iterator				vertex_iterator;
	typedef typename EDGES::const_iterator			const_edge_iterator;
	typedef typename EDGES::iterator				edge_iterator;

private:
	VERTICES					m_vertices;
	size_t						m_edge_count;

public:
	IC							graph			();
	virtual						~graph			();
	virtual void				save			(IWriter &stream);
	virtual void				load			(IReader &stream);
	IC		void				clear			();
	IC		void				add_vertex		(const _data_type &data, const _vertex_id_type &vertex_id);
	IC		void				remove_vertex	(const _vertex_id_type &vertex_id);
	IC		void				add_edge		(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight);
	IC		void				add_edge		(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight0, const _edge_weight_type &edge_weight1);
	IC		void				remove_edge		(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
	IC		size_t				vertex_count	() const;
	IC		size_t				edge_count		() const;
	IC		bool				empty			() const;
	IC		const VERTICES		&vertices		() const;
	IC		VERTICES			&vertices		();
	IC		const vertex_type	*vertex			(const _vertex_id_type &vertex_id) const;
	IC		vertex_type			*vertex			(const _vertex_id_type &vertex_id);
	IC		const edge_type		*edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1) const;
	IC		edge_type			*edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
	IC		const graph			&header			() const;
};

#include "graph_inline.h"