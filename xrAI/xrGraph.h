////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAI_XR_GRAPH__
#define __XRAI_XR_GRAPH__

#define GRAPH_VERSION		1
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
	u32					tNodeID:24;
	u32					tLevelID:8;
	u32					tVertexType:24;
	u32					tNeighbourCount:8;
	u32					dwEdgeOffset;
} SCompressedGraphVertex;

typedef struct tagSGraphHeader {
	u32					dwVersion;
	u32					dwVertexCount;
	u32					dwLevelCount;
} SGraphHeader;
#pragma pack(pop)

extern vector<SGraphVertex>	tpaGraph;		// graph

void xrBuildGraph(LPCSTR name);
void xrMergeSpawns();

#endif