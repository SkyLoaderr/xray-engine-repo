////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_cross_table.h
//	Created 	: 20.02.2003
//  Modified 	: 20.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#pragma once

#define CROSS_TABLE_NAME					"level.gct"

#define CROSS_TABLE_CHUNK_VERSION			0
#define CROSS_TABLE_CHUNK_DATA				1

class CALifeCrossTable {
public:
	#pragma pack(push,2)
	typedef struct tagSCrossTableHeader {
		u32									dwVersion;
		u32									dwNodeCount;
		u32									dwGraphPointCount;
	} SCrossTableHeader;
	
	typedef struct tagSCrossTableCell {
		u16									tGraphIndex;
		float								fDistance;
	} SCrossTableCell;
	#pragma pack(pop)

	SCrossTableHeader						m_tCrossTableHeader;
	CVirtualFileReader						*m_tpCrossTableVFS;
	SCrossTableCell							*m_tpaCrossTable;

	CALifeCrossTable						()
	{
		m_tpCrossTableVFS					= 0;
		m_tpaCrossTable						= 0;
	};

	CALifeCrossTable						(LPCSTR fName)
	{
		m_tpCrossTableVFS					= 0;
		m_tpaCrossTable						= 0;
		Load								(fName);
	};

	virtual ~CALifeCrossTable				()
	{
		Unload();
	};

	IC void Load							(LPCSTR fName)
	{
		m_tpCrossTableVFS					= xr_new<CVirtualFileReader>(fName);
		R_ASSERT							(m_tpCrossTableVFS);
		R_ASSERT							(m_tpCrossTableVFS->find_chunk(CROSS_TABLE_CHUNK_VERSION));
		m_tpCrossTableVFS->open_chunk		(CROSS_TABLE_CHUNK_VERSION);
		m_tCrossTableHeader.dwVersion		= m_tpCrossTableVFS->r_u32();
		m_tCrossTableHeader.dwNodeCount		= m_tpCrossTableVFS->r_u32();
		m_tCrossTableHeader.dwGraphPointCount	= m_tpCrossTableVFS->r_u32();
		R_ASSERT							(m_tCrossTableHeader.dwVersion == XRAI_CURRENT_VERSION);
		R_ASSERT							(m_tpCrossTableVFS->find_chunk(CROSS_TABLE_CHUNK_DATA));
		m_tpCrossTableVFS->open_chunk		(CROSS_TABLE_CHUNK_DATA);
		m_tpaCrossTable						= (SCrossTableCell*)m_tpCrossTableVFS->Pointer();
	};

	IC void Unload							()
	{
		xr_delete							(m_tpCrossTableVFS);
		m_tpaCrossTable						= 0;
	};
};