////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma pack(push,4)
typedef struct tagSGraphEdge {
	u32		dwVertexNumber;
	float	fPathDistance;
} SGraphEdge;

typedef struct tagSGraphVertex {
	Fvector				tLocalPoint;
	Fvector				tGlobalPoint;
	u32					tNodeID;
	u32					tVertexType;
	u32					tLevelID;
	u32					tNeighbourCount;
	tagSGraphEdge		*tpaEdges;
} SGraphVertex;

typedef struct tagSCompressedGraphVertex {
	Fvector				tLocalPoint;
	Fvector				tGlobalPoint;
	u8					tLevelID;
	u16					tVertexType;
	u8					tNeighbourCount;
	u32					dwEdgeOffset;
} SCompressedGraphVertex;

typedef struct tagSLevel {
	string256			caLevelName;
	Fvector				tOffset;
} SLevel;

typedef struct tagSGraphHeader {
	u32					dwVersion;
	u32					dwVertexCount;
	u32					dwLevelCount;
	vector<SLevel>		tpLevels;
} SGraphHeader;
#pragma pack(pop)

#define MAX_DISTANCE_TO_CONNECT		512.f

DEFINE_VECTOR(SGraphVertex,	GRAPH_VERTEX_VECTOR,	GRAPH_VERTEX_IT);
DEFINE_VECTOR(SGraphEdge,	GRAPH_EDGE_VECTOR,		GRAPH_EDGE_IT);

extern vector<SGraphVertex>	tpaGraph;		// graph
extern SGraphHeader			tGraphHeader;

void xrBuildGraph(LPCSTR name);
void xrMergeSpawns();