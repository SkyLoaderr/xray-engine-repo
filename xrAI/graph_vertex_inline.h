////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 27.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Data,\
	typename _edge_weight_type,\
	typename _vertex_id_type,\
	typename _vertex_index_type\
>

#define CSGraphVertex CGraphVertex<_Data,_edge_weight_type,_vertex_id_type,_vertex_index_type>

TEMPLATE_SPECIALIZATION
IC	CSGraphVertex::CGraphVertex													(
	const _Data				&data,
	const _vertex_id_type	vertex_id
) :
	m_data					(data),
	m_vertex_id				(vertex_id)
{
}

TEMPLATE_SPECIALIZATION
IC	CSGraphVertex::~CGraphVertex												()
{
	delete_data				(m_data);
}

TEMPLATE_SPECIALIZATION
IC	const typename CSGraphVertex::CSGraphEdge *CSGraphVertex::edge				(const _vertex_index_type vertex_index) const
{
	xr_vector<CSGraphEdge>::const_iterator	I = std::find_if(m_edges.begin(),m_edges.end(),vertex_index);
	if (m_edges.end() == I)
		return				(0);
	return					(&*I);
}

TEMPLATE_SPECIALIZATION
IC	typename CSGraphVertex::CSGraphEdge *CSGraphVertex::edge								(const _vertex_index_type vertex_index)
{
	xr_vector<CSGraphEdge>::iterator		I = std::find_if(m_edges.begin(),m_edges.end(),SEdgeFindPredicate(vertex_index));
	if (m_edges.end() == I)
		return				(0);
	return					(&*I);
}

TEMPLATE_SPECIALIZATION
template <typename _Stream>
IC	void CSGraphVertex::save													(_Stream &stream) const
{
	stream.w				(&m_data,sizeof(m_data));
	stream.w				(&m_vertex_index,sizeof(m_vertex_index));
	stream.w_u32			(m_edges.size());
	xr_vector<CSGraphEdge>::const_iterator I = m_edges.begin();
	xr_vector<CSGraphEdge>::const_iterator E = m_edges.end();
	for ( ; I != E; ++I)
		(*I).save			(stream);
}

TEMPLATE_SPECIALIZATION
template <typename _Stream>
IC	void CSGraphVertex::load													(_Stream &stream)
{
	stream.r				(&m_data,sizeof(m_data));
	stream.r				(&m_vertex_index,sizeof(m_vertex_index));
	m_edges.resize			(stream.r_u32());
	xr_vector<CSGraphEdge>::iterator I = m_edges.begin();
	xr_vector<CSGraphEdge>::iterator E = m_edges.end();
	for ( ; I != E; ++I)
		(*I).load			(stream);
}

TEMPLATE_SPECIALIZATION
IC	void CSGraphVertex::set_data												(_Data &data)
{
	m_data					= data;
}

TEMPLATE_SPECIALIZATION
IC	void CSGraphVertex::add_edge												(const _vertex_id_type vertex_id, const _vertex_index_type vertex_index, const _edge_weight_type edge_weight)
{
	xr_vector<CSGraphEdge>::iterator I = std::find(m_edges.begin(),m_edges.end(),vertex_index);
	VERIFY					(m_edges.end() == I);
	m_edges.push_back		(CSGraphEdge(edge_weight,vertex_id,vertex_index));
}

TEMPLATE_SPECIALIZATION
IC	void CSGraphVertex::remove_edge												(const _vertex_index_type vertex_index)
{
	xr_vector<CSGraphEdge>::iterator I = std::find(m_edges.begin(),m_edges.end(),vertex_index);
	VERIFY					(m_edges.end() != I);
	m_edges.erase			(I);
}

TEMPLATE_SPECIALIZATION
IC	void CSGraphVertex::update													(const _vertex_index_type vertex_index, u32 &edge_count)
{
	for (u32 i=0, n=edges().size(); i<n; ++i)
		if (edges()[i].vertex_index() == vertex_index) {
			m_edges.erase	(m_edges.begin() + i);
			--edge_count;
			--i;
			--n;
		}
		else
			if (edges()[i].vertex_index() > vertex_index)
				--(m_edges[i].m_vertex_index);

}

TEMPLATE_SPECIALIZATION
IC	const typename CSGraphVertex::_Data	&CSGraphVertex::data					() const
{
	return					(m_data);
}

TEMPLATE_SPECIALIZATION
IC	typename CSGraphVertex::_Data &CSGraphVertex::data							()
{
	return					(m_data);
}

TEMPLATE_SPECIALIZATION
IC	const typename CSGraphVertex::_vertex_id_type CSGraphVertex::vertex_id		() const
{
	return					(m_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<typename CSGraphVertex::CSGraphEdge> &CSGraphVertex::edges	() const
{
	return					(m_edges);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSGraphVertex