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
	VERIFY						(m_reader);
	m_header.load				(m_reader);
#ifndef AI_COMPILER
	R_ASSERT2					(header().version() == XRAI_CURRENT_VERSION,"Graph version mismatch!");
#else
	if (XRAI_CURRENT_VERSION != current_version)
		if (header().version() != current_version)
			return;
#endif
	m_nodes						= (CVertex*)m_reader->pointer();
	m_current_level_some_vertex_id = _GRAPH_ID(-1);
}

IC CGameGraph::~CGameGraph			()
{
	xr_delete					(m_reader);
}

IC const CGameGraph::CHeader &CGameGraph::header() const
{
	return						(m_header);
}

IC	bool CGameGraph::mask		(const svector<_LOCATION_ID,GameGraph::LOCATION_TYPE_COUNT> &M, const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const
{
	for (int i=0; i<GameGraph::LOCATION_TYPE_COUNT; ++i)
		if ((M[i] != E[i]) && (255 != M[i]))
			return(false);
	return(true);
}

IC	bool CGameGraph::mask		(const _LOCATION_ID M[GameGraph::LOCATION_TYPE_COUNT], const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const
{
	for (int i=0; i<GameGraph::LOCATION_TYPE_COUNT; ++i)
		if ((M[i] != E[i]) && (255 != M[i]))
			return(false);
	return(true);
}

IC	float CGameGraph::distance	(const _GRAPH_ID tGraphID0, const _GRAPH_ID tGraphID1) const
{
	const_iterator			i, e;
	begin					(tGraphID0,i,e);
	for ( ; i != e; ++i)
		if (value(tGraphID0,i) == tGraphID1)
			return				(edge_weight(i));
	R_ASSERT2					(false,"There is no proper graph point neighbour!");
	return						(_GRAPH_ID(-1));
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
	end							= (start = (const CEdge *)((BYTE *)m_nodes + vertex(_GRAPH_ID(vertex_id))->edge_offset())) + vertex(_GRAPH_ID(vertex_id))->edge_count();
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

IC	GameGraph::_LEVEL_ID GameGraph::CHeader::level_count() const
{
	return		(dwLevelCount);
}

IC	GameGraph::_GRAPH_ID GameGraph::CHeader::vertex_count() const
{
	return		((_GRAPH_ID)dwVertexCount);
}

IC	GameGraph::_GRAPH_ID GameGraph::CHeader::edge_count() const
{
	return		((_GRAPH_ID)dwEdgeCount);
}

IC	u32 GameGraph::CHeader::death_point_count() const
{
	return		(dwDeathPointCount);
}

IC	const GameGraph::LEVEL_MAP &GameGraph::CHeader::levels() const
{
	return		(tpLevels);
}

IC	const GameGraph::SLevel &GameGraph::CHeader::level	(const _LEVEL_ID &id) const
{
	LEVEL_MAP::const_iterator	I = levels().find(id);
	R_ASSERT2	(I != levels().end(),"There is no specified level in the game graph!");
	return		((*I).second);
}

IC	const GameGraph::SLevel &GameGraph::CHeader::level	(LPCSTR level_name) const
{
	LEVEL_MAP::const_iterator	I = levels().begin();
	LEVEL_MAP::const_iterator	E = levels().end();
	for ( ; I != E; ++I)
		if (!xr_strcmp((*I).second.name(),level_name))
			return	((*I).second);
	
#ifdef DEBUG
	Msg			("! There is no specified level %s in the game graph!",level_name);
	return		(levels().begin()->second);
#else
	R_ASSERT3	(false,"There is no specified level in the game graph!",level_name);
	NODEFAULT;
#endif
}

IC	const Fvector &GameGraph::CVertex::level_point() const
{
	return		(tLocalPoint);
}

IC	const Fvector &GameGraph::CVertex::game_point() const
{
	return		(tGlobalPoint);
}

IC	GameGraph::_LEVEL_ID GameGraph::CVertex::level_id() const
{
	return		(tLevelID);
}

IC	u32 GameGraph::CVertex::level_vertex_id() const
{
	return		(tNodeID);
}

IC	const u8 *GameGraph::CVertex::vertex_type() const
{
	return		(tVertexTypes);
}

IC	GameGraph::_GRAPH_ID GameGraph::CVertex::edge_count() const
{
	return		(tNeighbourCount);
}

IC	u32 GameGraph::CVertex::death_point_count() const
{
	return		(tDeathPointCount);
}

IC	u32	GameGraph::CVertex::edge_offset() const
{
	return		(dwEdgeOffset);
}

IC	u32	GameGraph::CVertex::death_point_offset() const
{
	return		(dwPointOffset);
}

IC	GameGraph::_GRAPH_ID GameGraph::CEdge::vertex_id() const
{
	return		((_GRAPH_ID)dwVertexNumber);
}

IC	float GameGraph::CEdge::distance() const
{
	return		(fPathDistance);
}

IC	void CGameGraph::begin_spawn(u32 vertex_id, const_spawn_iterator &start, const_spawn_iterator &end) const
{
	end = (start = (const_spawn_iterator)((u8*)m_nodes + vertex(vertex_id)->death_point_offset()) + vertex(vertex_id)->death_point_count());
}

IC	void CGameGraph::set_invalid_vertex(_GRAPH_ID &vertex_id) const
{
	vertex_id	= _GRAPH_ID(-1);
	VERIFY		(!valid_vertex_id(vertex_id));
}

IC	const GameGraph::_GRAPH_ID CGameGraph::vertex_id(const CGameGraph::CVertex *vertex) const
{
	VERIFY		(valid_vertex_id(_GRAPH_ID(vertex - m_nodes)));
	return		(_GRAPH_ID(vertex - m_nodes));
}
//
//IC	const _GRAPH_ID CGameGraph::nearest(const Fvector &position, u32 level_id) const
//{
//	float				min_dist_sqr = flt_max;
//	_GRAPH_ID	game_vertex_id = _GRAPH_ID(-1);
//	for (_GRAPH_ID i=0, n = header().vertex_count(); i<n; ++i) {
//		if (level_id != vertex(i)->level_id())
//			continue;
//
//		float			distance_sqr = vertex(i)->level_point().distance_to_sqr(position);
//		if (distance_sqr >= min_dist_sqr)
//			continue;
//
//		min_dist_sqr	= distance_sqr;
//		game_vertex_id	= i;
//	}
//	VERIFY				(valid_vertex_id(game_vertex_id));
//	return				(game_vertex_id);
//}

IC	void CGameGraph::set_current_level	(u32 level_id)
{
	m_current_level_some_vertex_id = _GRAPH_ID(-1);
	for (_GRAPH_ID i=0, n = header().vertex_count(); i<n; ++i) {
		if (level_id != vertex(i)->level_id())
			continue;

		m_current_level_some_vertex_id	= i;
		break;
	}
	VERIFY				(valid_vertex_id(m_current_level_some_vertex_id));
}

IC	const GameGraph::_GRAPH_ID	CGameGraph::current_level_vertex() const
{
	VERIFY				(valid_vertex_id(m_current_level_some_vertex_id));
	return				(m_current_level_some_vertex_id);
}

IC	void GameGraph::SLevel::load	(IReader *reader)
{
	reader->r_stringZ	(caLevelName);
	reader->r_fvector3	(tOffset);
	reader->r			(&tLevelID,sizeof(tLevelID));
	reader->r_stringZ	(m_section);
}

IC	void GameGraph::CHeader::load	(IReader *reader)
{
	dwVersion			= reader->r_u32();
	dwLevelCount		= reader->r_u32();
	dwVertexCount		= reader->r_u32();
	dwEdgeCount			= reader->r_u32();
	dwDeathPointCount	= reader->r_u32();
	tpLevels.clear		();
	for (u32 i=0; i<level_count(); ++i) {
		SLevel				l_tLevel;
		l_tLevel.load		(reader);
		tpLevels.insert		(mk_pair(l_tLevel.id(),l_tLevel));
	}
}
