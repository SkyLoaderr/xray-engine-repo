////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_graph.h
//	Created 	: 18.02.2003
//  Modified 	: 18.02.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life graph
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_space.h"

#define GRAPH_NAME							"game.graph"

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
		u32							dwVersion;
		u32							dwLevelCount;
		u32							dwVertexCount;
		u32							dwEdgeCount;
		u32							dwDeathPointCount;
		xr_vector<SLevel>			tpLevels;
	} SGraphHeader;
	#pragma pack(pop)

	SGraphHeader					m_tGraphHeader;	// graph header
	IReader							*m_tpGraphVFS;	// virtual file
	SGraphVertex					*m_tpaGraph;

									CSE_ALifeGraph		()
	{
		m_tpGraphVFS				= 0;
		m_tpaGraph					= 0;
	}

									CSE_ALifeGraph		(LPCSTR fName)
	{
		m_tpGraphVFS				= 0;
		m_tpaGraph					= 0;
		Load						(fName);
	}

	virtual							~CSE_ALifeGraph		()
	{
		xr_delete					(m_tpGraphVFS);
		m_tpaGraph					= 0;
	}

	IC const SGraphHeader&			GraphHeader			()
	{
		return						(m_tGraphHeader);
	}

			void					Load				(LPCSTR fName);

	IC		bool					bfCheckMask			(svector<ALife::_LOCATION_ID,LOCATION_TYPE_COUNT> &M, ALife::_LOCATION_ID E[LOCATION_TYPE_COUNT])
	{
		for (int i=0; i<LOCATION_TYPE_COUNT; i++)
			if ((M[i] != E[i]) && (M[i] != 255))
				return(false);
		return(true);
	}

	IC		float					ffGetDistanceBetweenGraphPoints(_GRAPH_ID tGraphID0, _GRAPH_ID tGraphID1)
	{
		SGraphEdge					*i = (SGraphEdge*)((BYTE*)m_tpaGraph + m_tpaGraph[tGraphID0].dwEdgeOffset);
		SGraphEdge					*e = i + m_tpaGraph[tGraphID0].tNeighbourCount;
		for ( ; i != e; i++)
			if (i->dwVertexNumber == tGraphID1)
				return				(i->fPathDistance);
		R_ASSERT2					(false,"There is no proper graph point neighbour!");
		return						(_GRAPH_ID(-1));
	}
};