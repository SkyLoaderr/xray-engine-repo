////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_abstract.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Abstract graph class
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Data,
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _vertex_index_type
>
class CAGraphAbstract {
public:
	struct CEdge {
		_edge_weight_type	m_weight;
		_vertex_id_type		m_vertex_id;
	};
	struct CVertex {
		_Data				m_data;
		_vertex_id_type		m_vertex_id;
		xr_vector<CEdge>	m_edges;
	};
private:
	xr_vector<CVertex>							m_vertices;
	xr_map<_vertex_id_type,_vertex_index_type>	m_index_by_id;
	xr_map<_vertex_index_type,_vertex_id_type>	m_id_by_index;
public:
	IC					CAGraphAbstract		();
	virtual				~CAGraphAbstract	();
	IC		void		add_vertex			(const _Data &data, const _vertex_id_type vertex_id);
	IC		void		remove_vertex		(const _vertex_id_type vertex_id);
	IC		void		add_edge			(const _Data &data, const _vertex_id_type vertex_id);
	IC		void		remove_vertex		(const _vertex_id_type vertex_id);
};

#include "graph_abstract_inline.h"