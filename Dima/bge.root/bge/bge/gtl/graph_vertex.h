////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex.h
//	Created 	: 14.01.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_edge.h"

template <
	typename _Data,
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _vertex_index_type
>
class CGraphVertex {
public:
	typedef _Data						_Data;
	typedef _edge_weight_type			_edge_weight_type;
	typedef _vertex_id_type				_vertex_id_type;
	typedef _vertex_index_type			_vertex_index_type;

	typedef CGraphEdge<
		_Data,
		_edge_weight_type,
		_vertex_id_type,
		_vertex_index_type
	>									CSGraphEdge;

	typedef CGraphVertex<
		_Data,
		_edge_weight_type,
		_vertex_id_type,
		_vertex_index_type
	>									CSGraphVertex;

private:
	_vertex_id_type						m_vertex_id;
	xr_vector<CSGraphEdge>				m_edges;
	_Data								m_data;

public:
	IC									CGraphVertex	(const _Data &data, const _vertex_id_type vertex_id);
	virtual								~CGraphVertex	();
	IC		const CSGraphEdge			*edge			(const _vertex_index_type vertex_index) const;
	IC		CSGraphEdge					*edge			(const _vertex_index_type vertex_index);
	IC		void						set_data		(_Data &data);
	IC		void						add_edge		(const _vertex_id_type vertex_id, const _vertex_index_type vertex_index, const _edge_weight_type edge_weight);
	IC		void						remove_edge		(const _vertex_index_type vertex_index);
	IC		void						update			(const _vertex_index_type vertex_index, u32 &edge_count);
	IC		const _Data					&data			() const;
	IC		_Data						&data			();
	IC		const _vertex_id_type		vertex_id		() const;
	IC		const xr_vector<CSGraphEdge>&edges			() const;
};

#include "graph_vertex_inline.h"