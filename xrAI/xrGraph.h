////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAI_XR_GRAPH__
#define __XRAI_XR_GRAPH__

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
#pragma pack(pop)

extern vector<SGraphVertex>	tpaGraph;		// graph

void xrBuildGraph(LPCSTR name);
void xrMergeSpawns();

#endif