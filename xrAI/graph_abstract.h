////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_abstract.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Abstract graph class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_edge.h"
#include "graph_vertex.h"

template <
	typename _Data,
	typename _edge_weight_type,
	typename _vertex_id_type,
	typename _vertex_index_type
>
class CGraphAbstract {
public:
	typedef CGraphEdge<
		_Data,
		typename _edge_weight_type,
		typename _vertex_id_type,
		typename _vertex_index_type
	>														CEdge;

	typedef CGraphVertex<
		typename _Data,
		typename _edge_weight_type,
		typename _vertex_id_type,
		typename _vertex_index_type
	>														CVertex;

public:
	typedef typename xr_vector<CEdge>::iterator				iterator;
	typedef typename xr_vector<CEdge>::const_iterator		const_iterator;
	typedef typename xr_vector<CVertex*>::iterator			vertex_iterator;
	typedef typename xr_vector<CVertex*>::const_iterator	const_vertex_iterator;

private:
	xr_vector<CVertex*>										m_vertices;
	xr_map<_vertex_id_type,_vertex_index_type>				m_index_by_id;
	u32														m_edge_count;

public:
	IC								CGraphAbstract			();
	virtual							~CGraphAbstract			();
	IC	void						add_vertex				(const _Data &data, const _vertex_id_type vertex_id);
	IC	void						remove_vertex			(const _vertex_id_type vertex_id);
	IC	void						add_edge				(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1, const _edge_weight_type edge_weight);
	IC	void						add_edge				(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1, const _edge_weight_type edge_weight0, const _edge_weight_type edge_weight1);
	IC	void						remove_edge				(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1);
	IC	_vertex_index_type			vertex_count			() const;
	IC	_vertex_index_type			edge_count				() const;
	IC	bool						empty					() const;
	IC	xr_vector<CVertex*>			&vertices				();
	IC	void						clear					();
	IC	const _vertex_index_type	vertex_index			(const _vertex_id_type vertex_id) const;
	IC	const CVertex				*vertex					(const _vertex_id_type vertex_id) const;
	IC	const CEdge					*edge					(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1) const;
	IC	CVertex						*vertex					(const _vertex_id_type vertex_id);
	IC	CEdge						*edge					(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1);
	IC	const xr_vector<CVertex*>	&vertices				() const;
	IC	const CGraphAbstract		&header					() const;
	IC	const _edge_weight_type		get_edge_weight			(const _vertex_index_type vertex_index0, const _vertex_index_type vertex_index1, const_iterator i) const;
	IC	bool						is_accessible			(const _vertex_index_type vertex_index) const;
	IC	const _vertex_index_type 	value					(const _vertex_index_type vertex_index, const_iterator i) const;
	IC	void						begin					(const _vertex_index_type vertex_index, const_iterator &b, const_iterator &e) const;

	template <typename _Stream>
	IC	void						save					(_Stream &stream) const;

	template <typename _Stream>
	IC	void						load					(_Stream &stream);
};

#include "graph_abstract_inline.h"