////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAI_XR_GRAPH__
#define __XRAI_XR_GRAPH__

typedef struct tagSGraphEdge {
	u32		dwVertexNumber;
	float	fPathDistance;
} SGraphEdge;

typedef struct tagSGraphVertex {
	Fvector				tPoint;
	u32					dwNodeID;
	unsigned char		ucVertexType;
	u32					dwNeighbourCount;
	tagSGraphEdge		*tpaEdges;
} SGraphVertex;

extern vector<SGraphVertex>	tpaGraph;		// graph

void xrBuildGraph(LPCSTR name);

#endif