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
#include "xrThread.h"
#include "xrGraph.h"
#include "ai_nodes.h"
#include "ai_a_star.h"

#define MAX_DISTANCE_TO_CONNECT		256.f
#define THREAD_COUNT				6

#define GET_INDEX(N,K)				iFloor((2*N - 1 - sqrtf((2*N - 1)*(2*N - 1) - 4*float(N)*(N - 1)/float(K)))/2.f)

#define START_THREADS(size,ThreadClass) {\
	u32	stride	= size/THREAD_COUNT;\
	u32	last	= size - stride*(THREAD_COUNT - 1);\
	for (u32 thID=0; thID<THREAD_COUNT; thID++)\
		tThreadManager.start(new ThreadClass(thID,thID*stride,thID*stride+((thID==(THREAD_COUNT - 1))?last:stride)));\
}

typedef struct tagRPoint {
	Fvector	P;
	Fvector A;
} RPoint;

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

u32 dwfErasePoints()
{
	u32 dwPointsWONodes = 0, dwTemp = 0;
	Progress(0.0f);
	for (int i=0; i<(int)tpaGraph.size(); 	Progress(float(++i)/tpaGraph.size()))
		if (tpaGraph[i].dwNodeID == u32(-1)) {
			Msg("Point %d [%7.2f,%7.2f,%7.2f] has no corresponding node",i + dwPointsWONodes++,tpaGraph[i].tPoint.x,tpaGraph[i].tPoint.y,tpaGraph[i].tPoint.z);
			tpaGraph.erase(tpaGraph.begin() + i--);
		}
	Progress(1.0f);
	return(dwPointsWONodes);
}

void xrBuildGraph(LPCSTR name)
{
	CThreadManager		tThreadManager;
	CCriticalSection	tCriticalSection;

	Msg("Building Level %s",name);

	Phase("Loading AI map");
	vfLoafAIMap(name);
	Msg("%d nodes loaded",int(m_header.count));
	
	tpaGraph.clear();

	Phase("Loading AI points");
	vfLoadAIPoints(name);
	u32 dwAIPoints;
	Msg("%d vertexes loaded",int(dwAIPoints = tpaGraph.size()));

	Phase("Initalizing AI path-finding structures");
	vfLoadSearch();

	Phase("Searching AI map for corresponding nodes");
	START_THREADS(tpaGraph.size(),CNodeThread);
	tThreadManager.wait();
	
	//Phase("Erasing points without corresponding nodes");
	Msg("%d points don't have corresponding nodes (they are deleted)",dwfErasePoints());

	Phase("Building graph");
	for (u32 thID=0, dwThreadCount = THREAD_COUNT, N = tpaGraph.size(), M = 0, K; thID<dwThreadCount; M += K, thID++)
		tThreadManager.start(new CGraphThread(thID,M, ((thID + 1) == dwThreadCount) ? N - 1 : M + (K = GET_INDEX((N - M),(dwThreadCount - thID))),MAX_DISTANCE_TO_CONNECT,tCriticalSection));
	tThreadManager.wait();
	for (int i=0, j=0; i<(int)tpaGraph.size(); i++)
		j += tpaGraph[i].dwNeighbourCount;
	Msg("%d edges built",j);

	Phase("Checking graph connectivity");
	if ((j < ((int)tpaGraph.size() - 1)) || !bfCheckForGraphConnectivity()) {
		Msg("Graph is not connected!");
		//R_ASSERT(false);
	}
	else
		Msg("Graph is connected");
	
	Phase("Saving graph");
	vfSaveGraph(name);

	Phase("Freeing graph being built");
	Progress(0.0f);
	for (i=0; i<(int)tpaGraph.size(); Progress(float(++i)/(tpaGraph.size() - 1)))
		_FREE(tpaGraph[i].tpaEdges);
	tpaGraph.clear();
	Progress(1.0f);
	
	Msg("\nBuilding level %s successfully completed",name);
}