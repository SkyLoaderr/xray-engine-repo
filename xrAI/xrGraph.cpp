////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGraph.cpp
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Building graph for off-line AI NPC computations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "levelgamedef.h"
#include "xrLevel.h"
#include "ai_nodes.h"
#include "ai_a_star.h"
#include "xrGraph.h"

#define MAX_DISTANCE_TO_CONNECT		100.f

typedef struct tagRPoint {
	Fvector	P;
	Fvector A;
} RPoint;

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

CStream*				vfs;			// virtual file
hdrNODES				m_header;		// m_header
BYTE*					m_nodes;		// virtual nodes DATA array
NodeCompressed**		m_nodes_ptr;	// pointers to node's data
vector<bool>			q_mark_bit;		// temporal usage mark for queries

vector<SGraphVertex>	tpaGraph;		// graph
stack<u32>				dwaStack;		// stack

void vfLoafAIMap(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.ai");
	vfs			= new CVirtualFileStream	(fName);
	
	// m_header & data
	vfs->Read	(&m_header,sizeof(m_header));
	R_ASSERT	(m_header.version == XRAI_CURRENT_VERSION);
	m_nodes		= (BYTE*) vfs->Pointer();

	// dispatch table
	m_nodes_ptr	= (NodeCompressed**)xr_malloc(m_header.count*sizeof(void*));
	Progress(0.0f);
	for (u32 I=0; I<m_header.count; Progress(float(++I)/m_header.count))
	{
		m_nodes_ptr[I]	= (NodeCompressed*)vfs->Pointer();

		NodeCompressed	C;
		vfs->Read		(&C,sizeof(C));

		u32			L = C.links;
		vfs->Advance	(L*sizeof(NodeLink));
	}

	// special query tables
	q_mark_bit.assign	(m_header.count,false);
	Progress(1.0f);
}

void vfLoadAIPoints(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.game");
	CVirtualFileStream	F(fName);

	CStream *O = 0;

	if (0!=(O = F.OpenChunk	(AIPOINT_CHUNK)))
	{
		for (int id=0; O->FindChunk(id); id++)
		{
			SGraphVertex					tGraphVertex;
			O->Rvector						(tGraphVertex.tPoint);
			tGraphVertex.dwNodeID			= 0;
			tGraphVertex.dwNeighbourCount	= 0;
			tGraphVertex.ucVertexType		= 0;
			tGraphVertex.tpaEdges			= 0;
			tpaGraph.push_back				(tGraphVertex);
			if (id % 100 == 0)
				Status("Vertexes being read : %d",id);
		}
		O->Close();
		Status("Vertexes being read : %d",id);
	}
}

u32 dwfFindCorrespondingNode(Fvector &tPoint)
{
	NodePosition	P;
	PackPosition	(P,tPoint);
	short min_dist	= 32767;
	int selected	= -1;
	for (int i=0; i<(int)m_header.count; i++) {
		NodeCompressed& N = *m_nodes_ptr[i];
		if (u_InsideNode(N,P)) {
			Fvector	DUP, vNorm, v, v1, P0;
			DUP.set(0,1,0);
			pvDecompress(vNorm,N.plane);
			Fplane PL; 
			UnpackPosition(P0,N.p0);
			PL.build(P0,vNorm);
			v.set(tPoint.x,P0.y,tPoint.z);	
			PL.intersectRayPoint(v,DUP,v1);
			int dist = iFloor((v1.y - tPoint.y)*(v1.y - tPoint.y));
			if (dist < min_dist) {
				min_dist = (short)dist;
				selected = i;
			}
		}
	}
	return(selected);
}

u32 dwfInitNodes()
{
	u32 dwPointsWONodes = 0;
	Progress(0.0f);
	for (int i=0; i<(int)tpaGraph.size(); 	Progress(float(++i)/tpaGraph.size()))
		if ((tpaGraph[i].dwNodeID = dwfFindCorrespondingNode(tpaGraph[i].tPoint)) == u32(-1))
			dwPointsWONodes++;
	Progress(1.0f);
	return(dwPointsWONodes);
}

void vfSaveGraph(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.graph");
	
	CFS_Memory	tGraph;
	tGraph.Wdword(m_header.version);	
	Progress(0.0f);
	for (int i=0; i<(int)tpaGraph.size(); Progress(float(++i)/tpaGraph.size())) {
		SGraphVertex &tGraphVertex = tpaGraph[i];
		tGraph.Wvector(tGraphVertex.tPoint);
		tGraph.Wbyte(tGraphVertex.ucVertexType);
		tGraph.Wdword(tGraphVertex.dwNodeID);	
		tGraph.Wdword(tGraphVertex.dwNeighbourCount);	
		for (int j=0; j<(int)tGraphVertex.dwNeighbourCount; j++) {
			tGraph.Wdword(tGraphVertex.tpaEdges[j].dwVertexNumber);	
			tGraph.Wfloat(tGraphVertex.tpaEdges[j].fPathDistance);	
		}
	}
	Progress(1.0f);
	tGraph.SaveTo(fName,0);
	Msg("%d bytes saved",int(tGraph.tell()));
}

void vfBuildGraph()
{
	float fDistance;
	u32 N = tpaGraph.size() - 1, M = N + 1, NM = N*M;
	Progress(0.0f);
	for (int i=0; i<(int)N; Progress(float(++i)/N)) {
		SGraphVertex &tCurrentGraphVertex = tpaGraph[i];
		for (int j = i + 1; j<(int)M; Progress((float(i)*N + ++j)/NM)) {
			SGraphVertex &tNeighbourGraphVertex = tpaGraph[j];
			if (tCurrentGraphVertex.tPoint.distance_to(tNeighbourGraphVertex.tPoint) < MAX_DISTANCE_TO_CONNECT) {
				vfFindTheShortestPath(tCurrentGraphVertex.dwNodeID,tNeighbourGraphVertex.dwNodeID,fDistance);
				if (fDistance < MAX_DISTANCE_TO_CONNECT) {
					tCurrentGraphVertex.tpaEdges = (SGraphEdge *)xr_realloc(tCurrentGraphVertex.tpaEdges,(++tCurrentGraphVertex.dwNeighbourCount)*sizeof(SGraphEdge));
					tCurrentGraphVertex.tpaEdges[tCurrentGraphVertex.dwNeighbourCount - 1].dwVertexNumber = j;
					tCurrentGraphVertex.tpaEdges[tCurrentGraphVertex.dwNeighbourCount - 1].fPathDistance  = fDistance;
					
					tNeighbourGraphVertex.tpaEdges = (SGraphEdge *)xr_realloc(tNeighbourGraphVertex.tpaEdges,(++tNeighbourGraphVertex.dwNeighbourCount)*sizeof(SGraphEdge));
					tNeighbourGraphVertex.tpaEdges[tNeighbourGraphVertex.dwNeighbourCount - 1].dwVertexNumber = i;
					tNeighbourGraphVertex.tpaEdges[tNeighbourGraphVertex.dwNeighbourCount - 1].fPathDistance  = fDistance;
				}
			}
		}
	}
	Progress(1.0f);
}

bool bfCheckForGraphConnectivity()
{
	if (!tpaGraph.size())
		return(true);

	while (dwaStack.size())
		dwaStack.pop();
	dwaStack.push(0);
	q_mark_bit[0] = true;
	while (!dwaStack.empty()) {
		u32 dwCurrentVertex = dwaStack.top();
		dwaStack.pop();
		SGraphVertex &tGraphVertex = tpaGraph[dwCurrentVertex];
		for (int i=0; i<(int)tGraphVertex.dwNeighbourCount; i++)
			if (!q_mark_bit[tGraphVertex.tpaEdges[i].dwVertexNumber]) {
				dwaStack.push(tGraphVertex.tpaEdges[i].dwVertexNumber);
				q_mark_bit[tGraphVertex.tpaEdges[i].dwVertexNumber] = true;
			}
	}
	for (int i=0; i<(int)tpaGraph.size(); i++)
		if (!q_mark_bit[i]) {
			q_mark_bit.assign(m_header.count,false);
			return(false);
		}
	
	q_mark_bit.assign(m_header.count,false);
	return(true);
}

void xrBuildGraph(LPCSTR name)
{
	Msg("Building Level %s",name);

	Phase("Loading AI map");
	vfLoafAIMap(name);
	Msg("%d nodes loaded",int(m_header.count));
	
	tpaGraph.clear();

	Phase("Loading AI points");
	vfLoadAIPoints(name);
	u32 dwAIPoints;
	Msg("%d vertexes loaded",int(dwAIPoints = tpaGraph.size()));

	Phase("Loading AI path-finding structures");
	vfLoadSearch();

	Phase("Searching AI map for corresponding nodes");
	Msg("%d points don't have corresponding nodes (they are deleted)",dwfInitNodes());

	Phase("Building graph");
	vfBuildGraph();
	for (int i=0, j=0; i<(int)tpaGraph.size(); i++)
		j += tpaGraph[i].dwNeighbourCount;
	Msg("%d edges built",j);

	Phase("Checking graph connectivity");
	if ((j < ((int)tpaGraph.size() - 1)) || !bfCheckForGraphConnectivity()) {
		Msg("Graph is not connected!");
		R_ASSERT(false);
	}
	else
		Msg("Graph is connected");
	
	Phase("Saving graph");
	vfSaveGraph(name);

	Phase("Freeing AI path-finding structures");
	vfUnloadSearch();
	
	Phase("Freeing graph being built");
	Progress(0.0f);
	for (i=0; i<(int)tpaGraph.size(); Progress(float(++i)/(tpaGraph.size() - 1)))
		_FREE(tpaGraph[i].tpaEdges);
	tpaGraph.clear();
	Progress(1.0f);
	
	Msg("\nBuilding level %s successfully completed",name);
}