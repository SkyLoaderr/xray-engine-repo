////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_abstract_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Abstract graph class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Data,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	typename _vertex_index_type\
>

#define CAbstractGraph CGraphAbstract<_Data,_edge_weight_type,_vertex_id_type,_vertex_index_type>

TEMPLATE_SPECIALIZATION
IC	CAbstractGraph::CGraphAbstract		()
{
}

TEMPLATE_SPECIALIZATION
CAbstractGraph::~CGraphAbstract			()
{
}

TEMPLATE_SPECIALIZATION
template <typename _Stream>
IC	void CAbstractGraph::save			(_Stream &stream) const
{
	stream.w_u32		(m_vertices.size());
	xr_vector<CVertex>::const_iterator	I = m_vertices.begin();
	xr_vector<CVertex>::const_iterator	E = m_vertices.end();
	for ( ; I != E; ++I)
		(*I).save		(stream);
}

TEMPLATE_SPECIALIZATION
template <typename _Stream>
IC	void CAbstractGraph::load			(_Stream &stream)
{
	m_vertices.resize	(stream.r_u32());
	xr_vector<CVertex>::iterator	I = m_vertices.begin();
	xr_vector<CVertex>::iterator	E = m_vertices.end();
	for ( ; I != E; ++I)
		(*I).load		(stream);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::add_vertex			(const _Data &data, const _vertex_id_type vertex_id)
{
	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id);
	VERIFY				(m_index_by_id.end() == I);
	m_index_by_id.insert(std::make_pair(vertex_id,m_vertices.size()));
	m_id_by_index.insert(std::make_pair(m_vertices.size(),vertex_id));
	m_vertices.push_back(CVertex(data,vertex_id));
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::remove_vertex		(const _vertex_id_type vertex_id)
{
	VERIFY				(vertex(vertex_id));
	xr_map<_vertex_id_type,_vertex_index_type>::iterator I = m_index_by_id.find(vertex_id);
	VERIFY				(m_index_by_id.end() != I);
	xr_map<_vertex_index_type,_vertex_id_type>::iterator J = m_id_by_index.find((*I).second);
	VERIFY				(m_id_by_index.end() != J);
	m_vertices[(*I).second].destroy();
	m_vertices.erase	(m_vertices.begin() + (*I).second);
	m_id_by_index.erase (J);
	m_index_by_id.erase (I);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::add_edge			(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1, const _edge_weight_type edge_weight)
{
	VERIFY				(vertex(vertex_id0));
	vertex(vertex_id0)->add_edge(vertex_id1,edge_weight);
	++m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::remove_edge		(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1)
{
	VERIFY				(m_edge_count);
	VERIFY				(vertex(vertex_id0));
	vertex(vertex_id0)->remove_edge(vertex_id1);
	--m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC	_vertex_index_type CAbstractGraph::vertex_count	() const
{
	return				(m_vertices.size());
}

TEMPLATE_SPECIALIZATION
IC	_vertex_index_type CAbstractGraph::edge_count	() const
{
	return				(m_edge_count);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractGraph::empty				() const
{
	return				(m_vertices.empty());
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
