////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_graph.h
//	Created 	: 18.02.2003
//  Modified 	: 18.02.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life graph
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef AI_COMPILER
	#include "xrLevel.h"
#else
	#include "..\\xrLevel.h"
#endif
#include "ai_alife_space.h"

#define GRAPH_NAME							"game.graph"

using namespace ALife;

class CALifeGraph {
public:
	#pragma pack(push,4)
	typedef struct tagSGraphEdge {
		u32									dwVertexNumber;
		float								fPathDistance;
	} SGraphEdge;

	typedef struct tagSGraphVertex {
		Fvector								tLocalPoint;
		Fvector								tGlobalPoint;
		u32									tLevelID:8;
		u32									tNodeID:24;
		u8									tVertexTypes[LOCATION_TYPE_COUNT];
		u32									tNeighbourCount:8;
		u32									dwEdgeOffset:24;
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
	IReader									*m_tpGraphVFS;	// virtual file
	SGraphVertex							*m_tpaGraph;

	CALifeGraph								()
	{
		m_tpGraphVFS						= 0;
		m_tpaGraph							= 0;
	};

	CALifeGraph								(LPCSTR fName)
	{
		m_tpGraphVFS						= 0;
		m_tpaGraph							= 0;
		Load								(fName);
	};

	virtual ~CALifeGraph					()
	{
		xr_delete							(m_tpGraphVFS);
		m_tpaGraph							= 0;
	};

	IC const SGraphHeader&					GraphHeader()
	{
		return								(m_tGraphHeader);
	};

	IC void Load							(LPCSTR fName)
	{
		m_tpGraphVFS					= xr_new<CVirtualFileStream>(fName);
		m_tGraphHeader.dwVersion		= m_tpGraphVFS->Rdword();
		m_tGraphHeader.dwVertexCount	= m_tpGraphVFS->Rdword();
		m_tGraphHeader.dwLevelCount		= m_tpGraphVFS->Rdword();
		m_tGraphHeader.tpLevels.resize	(m_tGraphHeader.dwLevelCount);
		{
			vector<SLevel>::iterator	I = m_tGraphHeader.tpLevels.begin();
			vector<SLevel>::iterator	E = m_tGraphHeader.tpLevels.end();
			for ( ; I != E; I++) {
				m_tpGraphVFS->RstringZ	((*I).caLevelName);
				m_tpGraphVFS->Rvector	((*I).tOffset);
			}
		}
		R_ASSERT						(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION);
		m_tpaGraph						= (SGraphVertex*)m_tpGraphVFS->Pointer();
	};

	IC	bool bfCheckMask(svector<_LOCATION_ID,LOCATION_TYPE_COUNT> &M, _LOCATION_ID E[LOCATION_TYPE_COUNT])
	{
		for (int i=0; i<LOCATION_TYPE_COUNT; i++)
			if ((M[i] != E[i]) && (M[i] != 255))
				return(false);
		return(true);
	};
};