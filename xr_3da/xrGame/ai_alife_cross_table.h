////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_cross_table.h
//	Created 	: 20.02.2003
//  Modified 	: 20.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#pragma once

#define CROSS_TABLE_CHUNK_VERSION			0
#define CROSS_TABLE_CHUNK_DATA				1

class CALifeCrossTable {
public:
	#pragma pack(push,2)
	typedef struct tagSCrossTableHeader {
		u32									dwVersion;
		u32									dwNodeCount;
		u32									dwGraphPointCount;
	} SCrossTabelHeader;
	
	typedef struct tagSCrossTableCell {
		u16									tGraphIndex;
		float								fDistance;
	} SCrossTableCell;
	#pragma pack(pop)

	SCrossTabelHeader						m_tCrossTableHeader;
	CVirtualFileStream						*m_tpCrossTableVFS;
	SCrossTableCell							*m_tpaCrossTable;

	CALifeCrossTable						()
	{
		m_tpCrossTableVFS					= 0;
		m_tpaCrossTable						= 0;
	};

	CALifeCrossTable						(const FILE_NAME &fName)
	{
		m_tpCrossTableVFS					= 0;
		m_tpaCrossTable						= 0;
		Load								(fName);
	};

	~CCrossTable							()
	{
		xr_delete							(m_tpCrossTableVFS);
	};

	IC void Load							(const FILE_NAME &fName)
	{
		m_tpCrossTableVFS					= xr_new<CVirtualFileStream>(fName);
		R_ASSERT							(m_tpCrossTableVFS);
		R_ASSERT							(m_tpCrossTableVFS->FindChunk(CROSS_TABLE_CHUNK_VERSION));
		m_tpCrossTableVFS->OpenChunk		(CROSS_TABLE_CHUNK_VERSION);
		m_tCrossTableHeader.dwVersion		= m_tpCrossTableVFS->Rdword();
		m_tCrossTableHeader.dwNodeCount		= m_tpCrossTableVFS->Rdword();
		m_tCrossTableHeader.dwGraphPointCount	= m_tpCrossTableVFS->Rdword();
		R_ASSERT							(m_tCrossTableHeader.dwVersion == XRAI_CURRENT_VERSION);
		R_ASSERT							(m_tpCrossTableVFS->FindChunk(CROSS_TABLE_CHUNK_DATA));
		m_tpCrossTableVFS->OpenChunk		(CROSS_TABLE_CHUNK_DATA);
		m_tpaCrossTable						= (SCrossTableCell*)m_tpCrossTableVFS->Pointer();
	};
};