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

class CSE_ALifeGraph {
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
		u32									tDeathPointCount:8;
		u32									dwPointOffset:24;
	} SGraphVertex;

	typedef struct tagSLevel {
		string256							caLevelName;
		Fvector								tOffset;
		u32									dwLevelID;
	} SLevel;

	typedef struct tagSGraphHeader {
		u32									dwVersion;
		u32									dwLevelCount;
		u32									dwVertexCount;
		u32									dwEdgeCount;
		u32									dwDeathPointCount;
		xr_vector<SLevel>					tpLevels;
	} SGraphHeader;
	#pragma pack(pop)

	SGraphHeader							m_tGraphHeader;	// graph header
	IReader									*m_tpGraphVFS;	// virtual file
	SGraphVertex							*m_tpaGraph;

	CSE_ALifeGraph								()
	{
		m_tpGraphVFS						= 0;
		m_tpaGraph							= 0;
	};

	CSE_ALifeGraph								(LPCSTR fName)
	{
		m_tpGraphVFS						= 0;
		m_tpaGraph							= 0;
		Load								(fName);
	};

	virtual ~CSE_ALifeGraph					()
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
		m_tpGraphVFS						= FS.r_open(fName);
		m_tGraphHeader.dwVersion			= m_tpGraphVFS->r_u32();
		m_tGraphHeader.dwLevelCount			= m_tpGraphVFS->r_u32();
		m_tGraphHeader.dwVertexCount		= m_tpGraphVFS->r_u32();
		m_tGraphHeader.dwEdgeCount			= m_tpGraphVFS->r_u32();
		m_tGraphHeader.dwDeathPointCount	= m_tpGraphVFS->r_u32();
		m_tGraphHeader.tpLevels.resize		(m_tGraphHeader.dwLevelCount);
		{
			xr_vector<SLevel>::iterator		I = m_tGraphHeader.tpLevels.begin();
			xr_vector<SLevel>::iterator		E = m_tGraphHeader.tpLevels.end();
			for ( ; I != E; I++) {
				m_tpGraphVFS->r_stringZ		((*I).caLevelName);
				m_tpGraphVFS->r_fvector3	((*I).tOffset);
				(*I).dwLevelID				= m_tpGraphVFS->r_u32();
			}
		}
		R_ASSERT2							(m_tGraphHeader.dwVersion == XRAI_CURRENT_VERSION,"Graph version mismatch!");
		m_tpaGraph							= (SGraphVertex*)m_tpGraphVFS->pointer();
	};

	IC	bool bfCheckMask(svector<_LOCATION_ID,LOCATION_TYPE_COUNT> &M, _LOCATION_ID E[LOCATION_TYPE_COUNT])
	{
		for (int i=0; i<LOCATION_TYPE_COUNT; i++)
			if ((M[i] != E[i]) && (M[i] != 255))
				return(false);
		return(true);
	};
};