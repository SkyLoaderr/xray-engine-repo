////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_base.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph base class template
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
//		4. specialize removals for different containers (back() instead of begin()) in graph
//		5. specialize removals for different containers (back() instead of begin()) in graph_vertex for edges
//		6. specialize removals for different containers (back() instead of begin()) in graph_vertex for vertices

template <
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _graph_vertex_type,
	typename _graph_edge_type
>
class graph_base : public IPureSerializeObject<IReader,IWriter> {
public:
	typedef _graph_vertex_type						vertex_type;
	typedef _graph_edge_type						edge_type;

public:
	typedef xr_map<_vertex_id_type,vertex_type*>	VERTICES;
	typedef typename vertex_type::EDGES				EDGES;

public:
	typedef typename VERTICES::const_iterator		const_vertex_iterator;
	typedef typename VERTICES::iterator				vertex_iterator;
	typedef typename EDGES::const_iterator			const_edge_iterator;
	typedef typename EDGES::iterator				edge_iterator;

private:
	VERTICES					m_vertices;
	size_t						m_edge_count;

protected:
	IC		VERTICES			&vertices			();

public:
	IC							graph_base			();
	virtual						~graph_base			();
	virtual void				save				(IWriter &stream);
	virtual void				load				(IReader &stream);
	IC		bool				operator==			(const graph &obj) const;
	IC		void				clear				();
	IC		void				remove_vertex		(const _vertex_id_type &vertex_id);
	IC		void				add_edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight);
	IC		void				add_edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1, const _edge_weight_type &edge_weight0, const _edge_weight_type &edge_weight1);
	IC		void				remove_edge			(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
	IC		size_t				vertex_count		() const;
	IC		size_t				edge_count			() const;
	IC		bool				empty				() const;
	IC		const VERTICES		&vertices			() const;
	IC		const vertex_type	*vertex				(const _vertex_id_type &vertex_id) const;
	IC		vertex_type			*vertex				(const _vertex_id_type &vertex_id);
	IC		const edge_type		*edge				(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1) const;
	IC		edge_type			*edge				(const _vertex_id_type &vertex_id0, const _vertex_id_type &vertex_id1);
	IC		const graph			&header				() const;

public:
	template <typename _vertex_allocator_type, typename T>
	IC		void				add_vertex			(const _vertex_id_type &vertex_id, _vertex_allocator_type &vertex_allocator, const T &data);
};

#include "graph_base_inline.h"