////////////////////////////////////////////////////////////////////////////
//	Module 		: xrCrossTable.h
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#pragma once

void xrBuildCrossTable(LPCSTR caProjectName);

#include "xrLevel.h"
#include "ai_alife_space.h"
typedef	char FILE_NAME[ _MAX_PATH	];

class CGraph {
public:
	#pragma pack(push,4)
	typedef struct tagSGraphEdge {
		u32									dwVertexNumber;
		float								fPathDistance;
	} SGraphEdge;

	typedef struct tagSGraphVertex {
		Fvector								tLocalPoint;
		Fvector								tGlobalPoint;
		u8									tLevelID;
		u32									tNodeID;
		u8									tVertexTypes[LOCATION_TYPE_COUNT];
		u8									tNeighbourCount;
		u32									dwEdgeOffset;
	} SGraphVertex;

	typedef struct tagSLevel {
		string256							caLevelName;
		Fvector								tOffset;
	} SLevel;

	typedef struct tagSGraphHeader {
		u32									dwVersion;
		u32									dwVertexCount;
		u32									dwLevelCount;
		vector<SLevel>						tpLevels;
	} SGraphHeader;
	#pragma pack(pop)

	SGraphHeader							m_tGraphHeader;	// graph header
	CVirtualFileStream						*m_tpGraphVFS;	// virtual file
	SGraphVertex							*m_tpaGraph;

	CGraph									()
	{
		m_tpGraphVFS						= 0;
		m_tpaGraph							= 0;
	};

	CGraph									(const FILE_NAME &fName)
	{
		m_tpGraphVFS						= 0;
		m_tpaGraph							= 0;
		Load								(fName);
	};

	~CGraph									()
	{
		xr_delete							(m_tpGraphVFS);
	};

	IC const SGraphHeader& Header			()
	{
		return								(m_tGraphHeader);
	};

	IC void Load							(const FILE_NAME &fName)
	{
		m_tpGraphVFS						= xr_new<CVirtualFileStream>(fName);
		m_tGraphHeader.dwVersion			= m_tpGraphVFS->Rdword();
		m_tGraphHeader.dwVertexCount		= m_tpGraphVFS->Rdword();
		m_tGraphHeader.dwLevelCount			= m_tpGraphVFS->Rdword();
		m_tGraphHeader.tpLevels.resize		(m_tGraphHeader.dwLevelCount);
		{
			vector<SLevel>::iterator		I = m_tGraphHeader.tpLevels.begin();
			vector<SLevel>::iterator		E = m_tGraphHeader.tpLevels.end();
			for ( ; I != E; I++) {
				m_tpGraphVFS->RstringZ		((*I).caLevelName);
				m_tpGraphVFS->Rvector		((*I).tOffset);
			}
		}
		R_ASSERT							(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
		m_tpaGraph							= (SGraphVertex*)m_tpGraphVFS->Pointer();
	};
};