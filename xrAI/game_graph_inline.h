////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_inline.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef AI_COMPILER
IC CGameGraph::CGameGraph		()
#else
IC CGameGraph::CGameGraph		(LPCSTR file_name, u32 current_version)
#endif
{
#ifndef AI_COMPILER
	string256					file_name;
	FS.update_path				(file_name,"$game_data$",GRAPH_NAME);
#endif	
	m_reader					= FS.r_open(file_name);
	m_header.dwVersion			= m_reader->r_u32();
	m_header.dwLevelCount		= m_reader->r_u32();
	m_header.dwVertexCount		= m_reader->r_u32();
	m_header.dwEdgeCount		= m_reader->r_u32();
	m_header.dwDeathPointCount	= m_reader->r_u32();
	m_header.tpLevels.clear		();
	for (u32 i=0; i<header().level_count(); ++i) {
		SLevel					l_tLevel;
		m_reader->r_stringZ		(l_tLevel.caLevelName);
		m_reader->r_fvector3	(l_tLevel.tOffset);
		m_reader->r				(&l_tLevel.tLevelID,sizeof(l_tLevel.tLevelID));
		if (header().version() > 7)
			m_reader->r_stringZ	(l_tLevel.m_section);
		m_header.tpLevels.insert(mk_pair(l_tLevel.tLevelID,l_tLevel));
	}
#ifndef AI_COMPILER
	R_ASSERT2					(header().version() == XRAI_CURRENT_VERSION,"Graph version mismatch!");
#else
	if (XRAI_CURRENT_VERSION != current_version)
		if (header().version() != current_version)
			return;
#endif
	m_nodes						= (CVertex*)m_reader->pointer();
}

IC CGameGraph::~CGameGraph			()
{
	xr_delete					(m_reader);
}

IC const CGameGraph::CHeader &CGameGraph::header() const
{
	return						(m_header);
}

IC	bool CGameGraph::mask		(const svector<ALife::_LOCATION_ID,LOCATION_TYPE_COUNT> &M, const ALife::_LOCATION_ID E[LOCATION_TYPE_COUNT]) const
{
	for (int i=0; i<LOCATION_TYPE_COUNT; ++i)
		if ((M[i] != E[i]) && (255 != M[i]))
			return(false);
	return(true);
}

IC	bool CGameGraph::mask		(const ALife::_LOCATION_ID M[LOCATION_TYPE_COUNT], const ALife::_LOCATION_ID E[LOCATION_TYPE_COUNT]) const
{
	for (int i=0; i<LOCATION_TYPE_COUNT; ++i)
		if ((M[i] != E[i]) && (255 != M[i]))
			return(false);
	return(true);
}

IC	float CGameGraph::distance	(const ALife::_GRAPH_ID tGraphID0, const ALife::_GRAPH_ID tGraphID1) const
{
	const_iterator			i, e;
	begin					(tGraphID0,i,e);
	for ( ; i != e; ++i)
		if (value(tGraphID0,i) == tGraphID1)
			return				(edge_weight(i));
	R_ASSERT2					(false,"There is no proper graph point neighbour!");
	return						(ALife::_GRAPH_ID(-1));
}

IC	bool CGameGraph::accessible	(const u32 vertex_id) const
{
	return						(true);
}

IC	bool CGameGraph::valid_vertex_id(const u32 vertex_id) const
{
	return						(vertex_id < header().vertex_count());
}

IC	void CGameGraph::begin		(const u32 vertex_id, const_iterator &start, const_iterator &end) const
{
	end							= (start = (const CEdge *)((BYTE *)m_nodes + vertex(ALife::_GRAPH_ID(vertex_id))->edge_offset())) + vertex(ALife::_GRAPH_ID(vertex_id))->edge_count();
}

IC	u32	 CGameGraph::value		(const u32 vertex_id, const_iterator &i) const
{
	return						(i->vertex_id());
}

IC	float CGameGraph::edge_weight(const_iterator i) const
{
	return						(i->distance());
}

IC	const CGameGraph::CVertex *CGameGraph::vertex(const u32 vertex_id) const
{
	return						(m_nodes + vertex_id);
}

IC	u32 CGameGraph::CHeader::version() const
{
	return						(dwVersion);
}

IC	ALife::_LEVEL_ID CGameGraph::CHeader::level_count() const
{
	return		(dwLevelCount);
}

IC	ALife::_GRAPH_ID CGameGraph::CHeader::vertex_count() const
{
	return		((ALife::_GRAPH_ID)dwVertexCount);
}

IC	ALife::_GRAPH_ID CGameGraph::CHeader::edge_count() const
{
	return		((ALife::_GRAPH_ID)dwEdgeCount);
}

IC	u32 CGameGraph::CHeader::death_point_count() const
{
	return		(dwDeathPointCount);
}

IC	const CGameGraph::LEVEL_MAP &CGameGraph::CHeader::levels() const
{
	return		(tpLevels);
}

IC	const CGameGraph::SLevel &CGameGraph::CHeader::level	(const ALife::_LEVEL_ID &id) const
{
	LEVEL_MAP::const_iterator	I = levels().find(id);
	R_ASSERT2	(I != levels().end(),"There is no specified level in the game graph!");
	return		((*I).second);
}

IC	const CGameGraph::SLevel &CGameGraph::CHeader::level	(LPCSTR level_name) const
{
	LEVEL_MAP::const_iterator	I = levels().begin();
	LEVEL_MAP::const_iterator	E = levels().end();
	for ( ; I != E; ++I)
		if (!xr_strcmp((*I).second.name(),level_name))
			return	((*I).second);
	
	R_ASSERT3	(false,"There is no specified level in the game graph!",level_name);
	NODEFAULT;
#ifdef DEBUG
	return		(levels().begin()->second);
#endif
}

IC	const Fvector &CGameGraph::CVertex::level_point() const
{
	return		(tLocalPoint);
}

IC	const Fvector &CGameGraph::CVertex::game_point() const
{
	return		(tGlobalPoint);
}

IC	ALife::_LEVEL_ID CGameGraph::CVertex::level_id() const
{
	return		(tLevelID);
}

IC	u32 CGameGraph::CVertex::level_vertex_id() const
{
	return		(tNodeID);
}

IC	const u8 *CGameGraph::CVertex::vertex_type() const
{
	return		(tVertexTypes);
}

IC	ALife::_GRAPH_ID CGameGraph::CVertex::edge_count() const
{
	return		(tNeighbourCount);
}

IC	u32 CGameGraph::CVertex::death_point_count() const
{
	return		(tDeathPointCount);
}

IC	u32	CGameGraph::CVertex::edge_offset() const
{
	return		(dwEdgeOffset);
}

IC	u32	CGameGraph::CVertex::death_point_offset() const
{
	return		(dwPointOffset);
}

IC	ALife::_GRAPH_ID CGameGraph::CEdge::vertex_id() const
{
	return		((ALife::_GRAPH_ID)dwVertexNumber);
}

IC	float CGameGraph::CEdge::distance() const
{
	return		(fPathDistance);
}

IC	void CGameGraph::begin_spawn(u32 vertex_id, const_spawn_iterator &start, const_spawn_iterator &end) const
{
	end = (start = (const_spawn_iterator)((u8*)m_nodes + vertex(vertex_id)->dwPointOffset) + vertex(vertex_id)->death_point_count());
}

IC	void CGameGraph::set_invalid_vertex(ALife::_GRAPH_ID &vertex_id) const
{
	vertex_id	= ALife::_GRAPH_ID(-1);
	VERIFY		(!valid_vertex_id(vertex_id));
}

IC	const ALife::_GRAPH_ID CGameGraph::vertex_id(const CGameGraph::CVertex *vertex) const
{
	VERIFY		(valid_vertex_id(ALife::_GRAPH_ID(vertex - m_nodes)));
	return		(ALife::_GRAPH_ID(vertex - m_nodes));
}
