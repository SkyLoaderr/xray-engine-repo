////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_cross_table_inline.h
//	Created 	: 20.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Cross table between game and level graphs inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef AI_COMPILER
IC CGameLevelCrossTable::CGameLevelCrossTable(LPCSTR fName, u32 current_version)
#else
IC CGameLevelCrossTable::CGameLevelCrossTable()
#endif
{
#ifndef AI_COMPILER
	string256							fName;
	FS.update_path						(fName,"$level$",CROSS_TABLE_NAME);
#endif
	m_tpCrossTableVFS					= FS.r_open(fName);
	R_ASSERT2							(m_tpCrossTableVFS,"Can't open cross table!");
	R_ASSERT2							(m_tpCrossTableVFS->find_chunk(CROSS_TABLE_CHUNK_VERSION),"Can't find chunk CROSS_TABLE_CHUNK_VERSION!");
	m_tpCrossTableVFS->open_chunk		(CROSS_TABLE_CHUNK_VERSION);
	m_tpCrossTableVFS->r				(&m_tCrossTableHeader,sizeof(m_tCrossTableHeader));
#ifdef AI_COMPILER
	if (XRAI_CURRENT_VERSION != current_version)
		if (header().version() != current_version)
			return;
#else
	R_ASSERT2							(m_tCrossTableHeader.version() == XRAI_CURRENT_VERSION,"Cross table version mismatch!");
#endif
	R_ASSERT2							(m_tpCrossTableVFS->find_chunk(CROSS_TABLE_CHUNK_DATA),"Can't find chunk CROSS_TABLE_CHUNK_DATA!");
	m_tpCrossTableVFS->open_chunk		(CROSS_TABLE_CHUNK_DATA);
	m_tpaCrossTable						= (CCell*)m_tpCrossTableVFS->pointer();
};

IC CGameLevelCrossTable::~CGameLevelCrossTable()
{
	xr_delete							(m_tpCrossTableVFS);
};

IC const CGameLevelCrossTable::CCell &CGameLevelCrossTable::vertex(u32 level_vertex_id) const
{
	return								(m_tpaCrossTable[level_vertex_id]);
}

IC	u32	CGameLevelCrossTable::CHeader::version() const
{
	return								(dwVersion);
}

IC	u32	CGameLevelCrossTable::CHeader::level_vertex_count() const
{
	return								(dwNodeCount);
}

IC	u32	CGameLevelCrossTable::CHeader::game_vertex_count() const
{
	return								(dwGraphPointCount);
}

IC	ALife::_GRAPH_ID CGameLevelCrossTable::CCell::game_vertex_id() const
{
	return			(tGraphIndex);
}

IC	float CGameLevelCrossTable::CCell::distance() const
{
	return			(fDistance);
}

IC	const CGameLevelCrossTable::CHeader &CGameLevelCrossTable::header() const
{
	return			(m_tCrossTableHeader);
}
