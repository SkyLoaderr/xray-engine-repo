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
	m_edge_count		= 0;
}

TEMPLATE_SPECIALIZATION
CAbstractGraph::~CGraphAbstract			()
{
	clear				();
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::save			(IWriter &stream)
{
	stream.open_chunk		(0);
	stream.w_u32			((u32)m_vertices.size());
	stream.close_chunk		();
	
	stream.open_chunk		(1);
	const_vertex_iterator	I = vertices().begin(), B = I;
	const_vertex_iterator	E = vertices().end();
	for ( ; I != E; ++I) {
		stream.open_chunk	(u32(I - B));

		{
			stream.open_chunk	(0);
			save_data			((*I)->data(),stream);
			stream.close_chunk	();

			stream.open_chunk	(1);
			save_data			((*I)->vertex_id(),stream);
			stream.close_chunk	();
		}

		stream.close_chunk	();
	}
	stream.close_chunk		();

	stream.open_chunk		(2);
	I						= B;
	for ( ; I != E; ++I) {
		stream.open_chunk	(u32(I - B));

		stream.w_u32		((*I)->edges().size());
		const_iterator		i = (*I)->edges().begin();
		const_iterator		e = (*I)->edges().end();
		for ( ; i != e; ++i) {
			save_data		((*i).vertex_id(),stream);
			save_data		((*i).weight(),stream);
		}

		stream.close_chunk	();
	}
	stream.close_chunk		();
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::load			(IReader &stream)
{
	clear						();

	_Data						data;
	_vertex_id_type				vertex_id;
	_edge_weight_type			edge_weight;
	IReader						*chunk0, *chunk1, *chunk2;

	chunk0						= stream.open_chunk(0);
	u32							n = chunk0->r_u32();

	chunk0						= stream.open_chunk(1);
	for (u32 i=0; i<n; ++i) {
		chunk1					= chunk0->open_chunk(i);
			
		{
			chunk2				= chunk1->open_chunk(0);
			load_data			(data,*chunk2);

			chunk2				= chunk1->open_chunk(1);
			load_data			(vertex_id,*chunk2);

			add_vertex			(data,vertex_id);
		}
	}

	chunk0						= stream.open_chunk(2);
	const_vertex_iterator		I = m_vertices.begin(), B = I;
	const_vertex_iterator		E = m_vertices.end();
	for ( ; I != E; ++I) {
		chunk1					= chunk0->open_chunk(u32(I - B));
		u32						n = chunk1->r_u32();
		for (u32 i=0; i<n; ++i) {
			load_data			(vertex_id,*chunk1);
			load_data			(edge_weight,*chunk1);
			add_edge			((*I)->vertex_id(),vertex_id,edge_weight);
		}
	}
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::add_vertex			(const _Data &data, const _vertex_id_type vertex_id)
{
	VERIFY				(vertex_count() < (u64(1) << (8*sizeof(_vertex_index_type))));

	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id);
	VERIFY				(m_index_by_id.end() == I);

	m_index_by_id.insert(std::make_pair(vertex_id,m_vertices.size()));
	m_vertices.push_back(xr_new<CVertex>(data,vertex_id));
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::remove_vertex		(const _vertex_id_type vertex_id)
{
	VERIFY				(vertex(vertex_id));
	
	xr_map<_vertex_id_type,_vertex_index_type>::iterator I = m_index_by_id.find(vertex_id);
	VERIFY				(m_index_by_id.end() != I);

	_vertex_index_type	vertex_index = (*I).second;

	m_edge_count		-= m_vertices[vertex_index]->edges().size();
	delete_data			(m_vertices[vertex_index]);
	m_vertices.erase	(m_vertices.begin() + vertex_index);
	m_index_by_id.erase (I);

	{
		xr_map<_vertex_id_type,_vertex_index_type>::iterator I = m_index_by_id.begin();
		xr_map<_vertex_id_type,_vertex_index_type>::iterator E = m_index_by_id.end();
		for ( ;I != E; ++I)
			if ((*I).second > vertex_index)
				--((*I).second);
	}
	{
		xr_vector<CVertex*>::iterator	I = m_vertices.begin();
		xr_vector<CVertex*>::iterator	E = m_vertices.end();
		for ( ; I != E; ++I)
			(*I)->update	(vertex_index,m_edge_count);
	}
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::add_edge			(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1, const _edge_weight_type edge_weight)
{
	VERIFY				(vertex(vertex_id0));
	xr_map<_vertex_id_type,_vertex_index_type>::iterator I = m_index_by_id.find(vertex_id1);
	VERIFY				(m_index_by_id.end() != I);
	vertex(vertex_id0)->add_edge(vertex_id1,(*I).second,edge_weight);
	++m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::add_edge			(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1, const _edge_weight_type edge_weight0, const _edge_weight_type edge_weight1)
{
	add_edge			(vertex_id0,vertex_id1,edge_weight0);
	add_edge			(vertex_id1,vertex_id0,edge_weight1);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::remove_edge		(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1)
{
	VERIFY				(m_edge_count);
	VERIFY				(vertex(vertex_id0));
	xr_map<_vertex_id_type,_vertex_index_type>::iterator I = m_index_by_id.find(vertex_id1);
	VERIFY				(m_index_by_id.end() != I);
	vertex(vertex_id0)->remove_edge((*I).second);
	--m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC	_vertex_index_type CAbstractGraph::vertex_count	() const
{
	return				(_vertex_index_type(m_vertices.size()));
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

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::clear				()
{
	while (!vertices().empty())
		remove_vertex(vertices().back()->vertex_id());
	VERIFY				(!m_edge_count);
}

TEMPLATE_SPECIALIZATION
IC	const _vertex_index_type CAbstractGraph::vertex_index(const _vertex_id_type vertex_id) const
{
	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id);
	VERIFY				(m_index_by_id.end() != I);
	VERIFY				((*I).second < m_vertices.size());
	return				((*I).second);
}

TEMPLATE_SPECIALIZATION
IC	const typename CAbstractGraph::CVertex *CAbstractGraph::vertex	(const _vertex_id_type vertex_id) const
{
	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id);
	if (m_index_by_id.end() == I)
		return			(0);
	VERIFY				((*I).second < m_vertices.size());
	return				(m_vertices[(*I).second]);
}

TEMPLATE_SPECIALIZATION
IC	const typename CAbstractGraph::CEdge *CAbstractGraph::edge	(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1) const
{
	const CVertex		*graph_vertex = vertex(vertex_id0);
	if (!graph_vertex)
		return			(0);
	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id1);
	VERIFY				(m_index_by_id.end() != I);
	return				(graph_vertex->edge((*I).second));
}

TEMPLATE_SPECIALIZATION
IC	typename CAbstractGraph::CVertex *CAbstractGraph::vertex	(const _vertex_id_type vertex_id)
{
	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id);
	if (m_index_by_id.end() == I)
		return			(0);
	VERIFY				((*I).second < m_vertices.size());
	return				(m_vertices[(*I).second]);
}

TEMPLATE_SPECIALIZATION
IC	typename CAbstractGraph::CEdge *CAbstractGraph::edge	(const _vertex_id_type vertex_id0, const _vertex_id_type vertex_id1)
{
	const CVertex		*graph_vertex = vertex(vertex_id0);
	if (!graph_vertex)
		return			(0);
	xr_map<_vertex_id_type,_vertex_index_type>::const_iterator I = m_index_by_id.find(vertex_id1);
	VERIFY				(m_index_by_id.end() != I);
	return				(graph_vertex->edge((*I).second));
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<typename CAbstractGraph::CVertex*> &CAbstractGraph::vertices() const
{
	return				(m_vertices);
}

TEMPLATE_SPECIALIZATION
IC	const CAbstractGraph &CAbstractGraph::header	() const
{
	return				(*this);
}

TEMPLATE_SPECIALIZATION
IC	const _edge_weight_type CAbstractGraph::get_edge_weight(const _vertex_index_type vertex_index0, const _vertex_index_type vertex_index1, const_iterator i) const
{
	VERIFY				(vertex_index0 < m_vertices.size());
	VERIFY				(vertex_index1 < m_vertices.size());
	VERIFY				(edge(m_vertices[vertex_index0]->vertex_id(),m_vertices[vertex_index1]->vertex_id()));
	return				((*i).weight());
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractGraph::is_accessible	(const _vertex_index_type vertex_index) const
{
	return				(true);
}

TEMPLATE_SPECIALIZATION
IC	const _vertex_index_type CAbstractGraph::value	(const _vertex_index_type vertex_index, const_iterator i) const
{
	VERIFY				((*i).vertex_index() < m_vertices.size());
	return				((*i).vertex_index());
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractGraph::begin	(const _vertex_index_type vertex_index, const_iterator &b, const_iterator &e) const
{
	VERIFY				(vertex_index < m_vertices.size());
	b					= m_vertices[vertex_index]->edges().begin();
	e					= m_vertices[vertex_index]->edges().end();
}

TEMPLATE_SPECIALIZATION
IC	xr_vector<typename CAbstractGraph::CVertex*> &CAbstractGraph::vertices	()
{
	return				(m_vertices);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager