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
class CGraphAbstract {
public:
	struct CEdge {
		_edge_weight_type	m_weight;
		_vertex_id_type		m_vertex_id;

		bool				operator()	(const CEdge &edge) const
		{
			return			(m_vertex_id == edge.m_vertex_id);
		}

		template <typename _Stream>
		IC	void			save(_Stream &stream) const
		{
			stream.w		(&m_weight,sizeof(m_weight));
			stream.w		(&m_vertex_id,sizeof(m_vertex_id));
		}

		template <typename _Stream>
		IC	void			load(_Stream &stream)
		{
			stream.r		(&m_weight,sizeof(m_weight));
			stream.r		(&m_vertex_id,sizeof(m_vertex_id));
		}
	};
	struct CVertex {
		_Data				m_data;
		_vertex_id_type		m_vertex_id;
		xr_vector<CEdge>	m_edges;

		IC	const CVertex	*vertex(const _vertex_id_type vertex_id) const
		{
			xr_vector<CEdge>::const_iterator I = std::find(m_edges.begin(),m_edges.end());
			if (m_edges.end() == I)
				return		(0);
			return			(*I);
		}

		template <typename _Stream>
			IC	void			save(_Stream &stream) const
		{
			stream.w		(&m_data,sizeof(m_data));
			stream.w		(&m_vertex_id,sizeof(m_vertex_id));
			stream.w_u32	(m_edges.size());
			xr_vector<CEdge>::const_iterator I = m_edges.begin();
			xr_vector<CEdge>::const_iterator E = m_edges.end();
			for ( ; I != E; ++I)
				(*I).save	(stream);
		}

		template <typename _Stream>
		IC	void			load(_Stream &stream)
		{
			stream.r		(&m_data,sizeof(m_data));
			stream.r		(&m_vertex_id,sizeof(m_vertex_id));
			m_edges.resize	(stream.r_u32());
			xr_vector<CEdge>::iterator I = m_edges.begin();
			xr_vector<CEdge>::iterator E = m_edges.end();
			for ( ; I != E; ++I)
				(*I).load	(stream);
		}
	};
private:
	xr_vector<CVertex>							m_vertices;
	xr_map<_vertex_id_type,_vertex_index_type>	m_index_by_id;
	xr_map<_vertex_index_type,_vertex_id_type>	m_id_by_index;
	u32											m_edge_count;
public:
	IC					CGraphAbstract		();
	virtual				~CGraphAbstract		();
	template <typename _Stream>
	IC		void		save				(_Stream &stream) const;
	template <typename _Stream>
	IC		void		load				(_Stream &stream);
	IC		void		add_vertex			(const _Data &data, const _vertex_id_type vertex_id);
	IC		void		remove_vertex		(const _vertex_id_type vertex_id);
	IC		void		add_edge			(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1, const _edge_weight_type edge_weight);
	IC		void		remove_edge			(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1);
	IC		_vertex_index_type vertex_count	() const;
	IC		_vertex_index_type edge_count	() const;


	IC		const CVertex *vertex			(const _vertex_id_type vertex_id) const
	{
		xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id);
		if (m_index_by_id.end() == I)
			return			(0);
		VERIFY				((*I).second < m_vertices.size());
		return				(&(m_vertices[(*I).second]));
	}

	IC		const CEdge *edge				(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1) const
	{
		const CVertex		*graph_vertex = vertex(vertex_id0);
		if (!graph_vertex)
			return			(0);
		return				(graph_vertex->edge(vertex_id1));
	}
};

#include "graph_abstract_inline.h"