////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_alife_space.h"
#include "ai_alife_graph.h"

typedef struct tagSDynamicGraphVertex {
	Fvector						tLocalPoint;
	Fvector						tGlobalPoint;
	u32							tNodeID;
	u8							tVertexTypes[LOCATION_TYPE_COUNT];
	u32							tLevelID;
	u32							tNeighbourCount;
	CALifeGraph::SGraphEdge		*tpaEdges;
} SDynamicGraphVertex;

#define MAX_DISTANCE_TO_CONNECT		512.f

DEFINE_VECTOR(SDynamicGraphVertex,		GRAPH_VERTEX_VECTOR,	GRAPH_VERTEX_IT);
DEFINE_VECTOR(CALifeGraph::SGraphEdge,	GRAPH_EDGE_VECTOR,		GRAPH_EDGE_IT);

extern vector<SDynamicGraphVertex>	tpaGraph;		// graph
extern CALifeGraph::SGraphHeader	tGraphHeader;

void xrBuildGraph(LPCSTR name);
void xrMergeSpawns();