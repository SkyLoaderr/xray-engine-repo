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

#include "ai_nodes.h"
#include "xrGraph.h"
#include "xrSort.h"

#define THREAD_COUNT				1

#define GET_INDEX(N,K)				iFloor((2*N - 1 - _sqrt((2*N - 1)*(2*N - 1) - 4*float(N)*(N - 1)/float(K)))/2.f)

#define START_THREADS(size,ThreadClass) {\
	u32	stride	= size/THREAD_COUNT;\
	u32	last	= size - stride*(THREAD_COUNT - 1);\
	for (u32 thID=0; thID<THREAD_COUNT; thID++)\
		tThreadManager.start(xr_new<ThreadClass>(thID,thID*stride,thID*stride+((thID==(THREAD_COUNT - 1))?last:stride),tpAI_Map));\
}

typedef struct tagRPoint {
	Fvector	P;
	Fvector A;
} RPoint;

SGraphHeader			tGraphHeader;
vector<SGraphVertex>	tpaGraph;		// graph
SGraphEdge				*tpaEdges;		// graph edges
SGraphEdge				*tpaFullEdges;	// graph edges
stack<u32>				dwaStack;		// stack
u32						*dwaSortOrder;  // edge sort order
u32						*dwaEdgeOwner;  // edge owners

void vfLoadAIPoints(LPCSTR name)
{
	string256			fName;
	strconcat			(fName,name,"level.game");
	CVirtualFileStream	F(fName);

	CStream *O = 0;

	if (0!=(O = F.OpenChunk	(AIPOINT_CHUNK)))
	{
		for (int id=0; O->FindChunk(id); id++)
		{
			SGraphVertex					tGraphVertex;
			O->Rvector						(tGraphVertex.tLocalPoint);
			tGraphVertex.tGlobalPoint		= tGraphVertex.tLocalPoint;
			tGraphVertex.tNodeID			= 0;
			tGraphVertex.tNeighbourCount	= 0;
			tGraphVertex.tVertexType		= 0;
			tGraphVertex.tLevelID			= 0;
			tGraphVertex.tpaEdges			= 0;
			tpaGraph.push_back				(tGraphVertex);
			if (id % 100 == 0)
				Status("Vertexes being read : %d",id);
		}
		O->Close();
		Status("Vertexes being read : %d",id);
	}
}

void vfRemoveDuplicateAIPoints()
{
	Progress(0.0f);
	u32 N = tpaGraph.size(), *dwpSortOrder = (u32 *)xr_malloc(N*sizeof(u32)), *dwpGraphOrder  = (u32 *)xr_malloc(N*sizeof(u32));
	for (int i=0; i<(int)N; i++)
		dwpSortOrder[i] = i;
	bool bOk;
	do {
		bOk = true;
		for ( i=1; i<(int)N; i++) {
			Fvector &p1 = tpaGraph[dwpSortOrder[i - 1]].tLocalPoint;
			Fvector &p2 = tpaGraph[dwpSortOrder[i]].tLocalPoint;
			if ((p1.x > p2.x) || ((_abs(p1.x - p2.x) < EPS_L) && ((p1.y > p2.y) || ((_abs(p1.y - p2.y) < EPS_L) && (p1.z > p2.z))))) {
				int k = dwpSortOrder[i - 1];
				dwpSortOrder[i - 1] = dwpSortOrder[i];
				dwpSortOrder[i] = k;
				bOk = false;
			}
		}
	}
	while (!bOk);

	int j = 0;
	for ( i=1; i<(int)N; i++) {
		Fvector &p1 = tpaGraph[dwpSortOrder[i - 1]].tLocalPoint;
		Fvector &p2 = tpaGraph[dwpSortOrder[i]].tLocalPoint;
		if ((_abs(p1.x - p2.x) < EPS_L) && (_abs(p1.y - p2.y) < EPS_L) && (_abs(p1.z - p2.z) < EPS_L))
			dwpGraphOrder[j++] = dwpSortOrder[i];
	}
	
	do {
		bOk = true;
		for ( i=1; i<j; i++) {
			if (dwpGraphOrder[i - 1] > dwpGraphOrder[i]) {
				int k = dwpGraphOrder[i - 1];
				dwpGraphOrder[i - 1] = dwpGraphOrder[i];
				dwpGraphOrder[i] = k;
				bOk = false;
			}
		}
	}
	while (!bOk);

	for (i=0; i<j; i++)
		tpaGraph.erase(tpaGraph.begin() + dwpGraphOrder[i] - i);
	xr_free(dwpSortOrder);
	xr_free(dwpGraphOrder);
	Progress(1.0f);
	Msg("%d vertexes has been removed",j);
}

bool bfCheckForGraphConnectivity(CAI_Map *tpAI_Map)
{
	if (!tpaGraph.size())
		return(true);

	while (dwaStack.size())
		dwaStack.pop();
	dwaStack.push(0);
	tpAI_Map->q_mark_bit[0] = true;
	while (!dwaStack.empty()) {
		u32 dwCurrentVertex = dwaStack.top();
		dwaStack.pop();
		SGraphVertex &tGraphVertex = tpaGraph[dwCurrentVertex];
		for (int i=0; i<(int)tGraphVertex.tNeighbourCount; i++)
			if (!tpAI_Map->q_mark_bit[tGraphVertex.tpaEdges[i].dwVertexNumber]) {
				dwaStack.push(tGraphVertex.tpaEdges[i].dwVertexNumber);
				tpAI_Map->q_mark_bit[tGraphVertex.tpaEdges[i].dwVertexNumber] = true;
			}
	}
	for (int i=0; i<(int)tpaGraph.size(); i++)
		if (!tpAI_Map->q_mark_bit[i]) {
			tpAI_Map->q_mark_bit.assign(tpAI_Map->m_header.count,false);
			return(false);
		}
	
	tpAI_Map->q_mark_bit.assign(tpAI_Map->m_header.count,false);
	return(true);
}

u32 dwfErasePoints()
{
	u32 dwPointsWONodes = 0, dwTemp = 0;
	Progress(0.0f);
	for (int i=0; i<(int)tpaGraph.size(); 	Progress(float(++i)/tpaGraph.size()))
		if (tpaGraph[i].tNodeID == u32(-1)) {
			Msg("Point %3d [%7.2f,%7.2f,%7.2f] has no corresponding node",i + dwPointsWONodes++,tpaGraph[i].tLocalPoint.x,tpaGraph[i].tLocalPoint.y,tpaGraph[i].tLocalPoint.z);
			tpaGraph.erase(tpaGraph.begin() + i--);
		}
	Progress(1.0f);
	return(dwPointsWONodes);
}

void vfAllocateMemory()
{
	Progress(0.0f);
	int N = tpaGraph.size();
	tpaFullEdges = (SGraphEdge *)xr_malloc(N*(N - 1)*sizeof(SGraphEdge));
	SGraphEdge	*tpPointer = tpaFullEdges;
	for (int i=0; i<N; tpPointer += (N - 1), i++)
		tpaGraph[i].tpaEdges = tpPointer;
	Msg("* %d memory allocated",N*(N - 1)*sizeof(SGraphEdge));
	Progress(1.0f);
}

void vfPreprocessEdges(u32 dwEdgeCount)
{
	Progress(0.0f);
	SGraphEdge *tpPointer = tpaEdges = (SGraphEdge *)xr_malloc(dwEdgeCount*sizeof(SGraphEdge));
	dwaSortOrder = (u32 *)xr_malloc(dwEdgeCount*sizeof(u32));
	dwaEdgeOwner = (u32 *)xr_malloc(dwEdgeCount*sizeof(u32));
	for (int i=0, j=0; i<(int)tpaGraph.size(); i++) {
		SGraphVertex &tGraphVertex = tpaGraph[i]; 
		Memory.mem_copy(tpPointer,tGraphVertex.tpaEdges,tGraphVertex.tNeighbourCount*sizeof(SGraphEdge));
		//PSGP.memCopy(tpPointer,tGraphVertex.tpaEdges,tGraphVertex.tNeighbourCount*sizeof(SGraphEdge));
		//xr_free(tGraphVertex.tpaEdges);
		tGraphVertex.tpaEdges = tpPointer;
		tpPointer += tGraphVertex.tNeighbourCount;
		for (int k=0; k<(int)tGraphVertex.tNeighbourCount; k++, j++) {
			dwaSortOrder[j] = j;
			dwaEdgeOwner[j] = i;
		}
	}
	xr_free(tpaFullEdges);
	Progress(1.0f);
}

void vfOptimizeGraph(u32 dwEdgeCount, CAI_Map *tpAI_Map)
{
	tpAI_Map->q_mark_bit.resize(dwEdgeCount);
	tpAI_Map->q_mark_bit.assign(dwEdgeCount,false);
	Progress(0.0f);
	for (int i=dwEdgeCount - 1; i>=0; Progress((float(dwEdgeCount) - (i--))/dwEdgeCount)) {
		if (tpAI_Map->q_mark_bit[dwaSortOrder[i]])
			continue;
		float fDistance = tpaEdges[dwaSortOrder[i]].fPathDistance;
		u32 dwVertex0 = dwaEdgeOwner[dwaSortOrder[i]];
		u32 dwVertex1 = tpaEdges[dwaSortOrder[i]].dwVertexNumber;
		SGraphVertex &tVertex0 = tpaGraph[dwVertex0];
		SGraphVertex &tVertex1 = tpaGraph[dwVertex1];
		bool bOk = true;
		for (int i0=0; (i0<(int)tVertex0.tNeighbourCount) && bOk; i0++) {
			if (tpAI_Map->q_mark_bit[tVertex0.tpaEdges + i0 - tpaEdges])
				continue;
			SGraphEdge &tEdge0 = tVertex0.tpaEdges[i0];
			for (int i1=0; i1<(int)tVertex1.tNeighbourCount; i1++)
				if ((tEdge0.dwVertexNumber == tVertex1.tpaEdges[i1].dwVertexNumber) && !tpAI_Map->q_mark_bit[tVertex1.tpaEdges + i1 - tpaEdges]) {
					tpAI_Map->q_mark_bit[dwaSortOrder[i]] = true;
					for (int j=0; j<(int)tVertex1.tNeighbourCount; j++)
						if (tVertex1.tpaEdges[j].dwVertexNumber == dwVertex0) {
							tpAI_Map->q_mark_bit[tVertex1.tpaEdges + j - tpaEdges] = true;
							break;
						}
					bOk = false;
					break;
				}
		}
	}
	Progress(1.0f);
}

void vfNormalizeGraph()
{
	for (int i=0, N=(int)tpaGraph.size(); i<N; i++) {
		bool bOk;
		do {
			bOk = true;
			for (int j=1; j<(int)tpaGraph[i].tNeighbourCount; j++)
				if (tpaGraph[i].tpaEdges[j - 1].dwVertexNumber > tpaGraph[i].tpaEdges[j].dwVertexNumber) {
					SGraphEdge tTemp = tpaGraph[i].tpaEdges[j - 1];
					tpaGraph[i].tpaEdges[j - 1] = tpaGraph[i].tpaEdges[j];
					tpaGraph[i].tpaEdges[j] = tTemp;
					bOk = false;
				}
		} 
		while(!bOk);
	}

}

void vfSaveGraph(LPCSTR name, CAI_Map *tpAI_Map)
{
	string256					fName;
	strconcat					(fName,name,"level.graph");
	
	CFS_Memory					tGraph;
	tGraphHeader.dwVersion		= XRAI_CURRENT_VERSION;
	tGraphHeader.dwVertexCount	= tpaGraph.size();
	tGraphHeader.dwLevelCount	= 1;
	SLevel						tLevel;
	tLevel.tOffset.set			(0,0,0);
	Memory.mem_copy(tLevel.caLevelName,name,strlen(name) + 1);
	tGraphHeader.tpLevels.push_back(tLevel);
	tGraph.Wdword				(tGraphHeader.dwVersion);
	tGraph.Wdword				(tGraphHeader.dwVertexCount);
	tGraph.Wdword				(tGraphHeader.dwLevelCount);
	vector<SLevel>::iterator	I = tGraphHeader.tpLevels.begin();
	vector<SLevel>::iterator	E = tGraphHeader.tpLevels.end();
	for ( ; I != E; I++) {
		tGraph.WstringZ((*I).caLevelName);
		tGraph.Wvector((*I).tOffset);
	}

	u32		dwPosition = tGraph.size();
	
	Progress(0.0f);
	SCompressedGraphVertex tCompressedGraphVertex;
	Memory.mem_fill(&tCompressedGraphVertex,0,sizeof(SCompressedGraphVertex));
	for (int i=0; i<(int)tpaGraph.size(); Progress(float(++i)/tpaGraph.size()/4))
		tGraph.write(&tCompressedGraphVertex,sizeof(SCompressedGraphVertex));
	
	Progress(0.25f);
	for (int i=0; i<(int)tpaGraph.size(); Progress(.25f + float(++i)/tpaGraph.size()/2)) {
		SGraphVertex &tGraphVertex = tpaGraph[i];
		for (int j=0, k=0; j<(int)tGraphVertex.tNeighbourCount; j++)
			if (!tpAI_Map->q_mark_bit[tGraphVertex.tpaEdges + j - tpaEdges]) {
				k++;
				tGraph.Wdword(tGraphVertex.tpaEdges[j].dwVertexNumber);	
				tGraph.Wfloat(tGraphVertex.tpaEdges[j].fPathDistance);	
			}
		tGraphVertex.tNeighbourCount = k;
	}
	
	Progress(.75f);
	tGraph.seek(dwPosition);
	for (int i=0, j=0, k=tpaGraph.size()*sizeof(SCompressedGraphVertex); i<(int)tpaGraph.size(); j += tpaGraph[i].tNeighbourCount, Progress(.75f + float(++i)/tpaGraph.size()/4)) {
		SGraphVertex &tGraphVertex				= tpaGraph[i];
		tCompressedGraphVertex.tLocalPoint		= tGraphVertex.tLocalPoint;
		tCompressedGraphVertex.tGlobalPoint		= tGraphVertex.tGlobalPoint;
		//tCompressedGraphVertex.tNodeID			= tGraphVertex.tNodeID;
		tCompressedGraphVertex.tVertexType		= tGraphVertex.tVertexType;
		tCompressedGraphVertex.tLevelID			= tGraphVertex.tLevelID;
		tCompressedGraphVertex.tNeighbourCount = tGraphVertex.tNeighbourCount;
		tCompressedGraphVertex.dwEdgeOffset		= k + j*sizeof(SGraphEdge);
		tGraph.write(&tCompressedGraphVertex,	sizeof(SCompressedGraphVertex));
	}
	tGraph.SaveTo(fName,0);
	Progress(1.0f);
	Msg("%d bytes saved",int(tGraph.size()));
}

void xrBuildGraph(LPCSTR name)
{
	CThreadManager		tThreadManager;		// multithreading
	xrCriticalSection	tCriticalSection;	// thread synchronization
	CAI_Map				*tpAI_Map;

	Msg("Building Level %s",name);

	Phase("Loading AI map");
	Progress(0.0f);
	tpAI_Map = xr_new<CAI_Map>(name);
	Progress(1.0f);
	Msg("%d nodes loaded",int(tpAI_Map->m_header.count));
	
	tpaGraph.clear();

	Phase("Loading AI points");
	vfLoadAIPoints(name);
	u32 dwAIPoints;
	Msg("%d vertexes loaded",int(dwAIPoints = tpaGraph.size()));

	Phase("Removing duplicate AI points");
	vfRemoveDuplicateAIPoints();

	Phase("Searching AI map for corresponding nodes");
	START_THREADS(tpaGraph.size(),CNodeThread);
	tThreadManager.wait();
	Progress(1.0f);
	Msg("%d points don't have corresponding nodes (they are removed)",dwfErasePoints());

	Phase("Allocating memory");
	vfAllocateMemory();

	Phase("Building graph");
	for (u32 thID=0, dwThreadCount = THREAD_COUNT, N = tpaGraph.size(), M = 0, K = 0; thID<dwThreadCount; M += K, thID++)
		tThreadManager.start(xr_new<CGraphThread>(thID,M, ((thID + 1) == dwThreadCount) ? N - 1 : M + (K = GET_INDEX((N - M),(dwThreadCount - thID))),MAX_DISTANCE_TO_CONNECT,&tCriticalSection,*tpAI_Map));
	tThreadManager.wait();
	
	for (int i=0, dwEdgeCount=0; i<(int)tpaGraph.size(); i++)
		dwEdgeCount += tpaGraph[i].tNeighbourCount;
	Msg("%d edges built",dwEdgeCount);
	Progress(1.0f);

	Phase("Normalizing graph");
	vfNormalizeGraph();

	Phase("Checking graph connectivity");
	if ((dwEdgeCount < ((int)tpaGraph.size() - 1)) || !bfCheckForGraphConnectivity(tpAI_Map)) {
		Msg("Graph is not connected!");
		extern  HWND logWindow;
		MessageBox	(logWindow,"Graph is not connected","Data mismatch",MB_OK|MB_ICONWARNING);
		return;
	}
	else
		Msg("Graph is connected");
	
	Phase("Preprocessing edges");
	vfPreprocessEdges(dwEdgeCount);

	Phase("Sorting edges");
	Progress(0.0f);
	vfQuickSortEdges(tpaEdges,dwaSortOrder,dwEdgeCount);
	Progress(1.0f);

	Phase("Optimizing graph");
	vfOptimizeGraph(dwEdgeCount,tpAI_Map);

	Phase("Saving graph");
	vfSaveGraph(name,tpAI_Map);
	for (int i=0, dwNewEdgeCount=0; i<(int)tpaGraph.size(); i++)
		dwNewEdgeCount += tpaGraph[i].tNeighbourCount;
	Msg("%d edges are removed",dwEdgeCount - dwNewEdgeCount);

	Phase("Freeing graph being built");
	Progress(0.0f);
	xr_free(tpaEdges);
	xr_free(dwaSortOrder);
	xr_delete(tpAI_Map);
	tpaGraph.clear();
	Progress(1.0f);
	
	Msg("\nBuilding level %s successfully completed",name);
}
