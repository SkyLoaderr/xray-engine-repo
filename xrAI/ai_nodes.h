////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_nodes.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : AI nodes structures and functions
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAI_AI_NODES__
#define __XRAI_AI_NODES__

#include "compiler.h"
#include "xrThread.h"
#include "xrSyncronize.h"

#include "xrGraph.h"
#include "ai_nodes.h"
#include "ai_a_star.h"

extern CStream*					vfs;			// virtual file
extern hdrNODES					m_header;		// m_header
extern BYTE*					m_nodes;		// virtual nodes DATA array
extern NodeCompressed**			m_nodes_ptr;	// pointers to node's data
extern vector<bool>				q_mark_bit;		// temporal usage mark for queries

#define MAX_VALUE				1000000.0

IC	NodeCompressed*		Node(u32 ID)	{ return vfs?m_nodes_ptr[ID]:NULL; }

IC	u32		UnpackLink		(NodeLink& L) {	return (*LPDWORD(&L))&0x00ffffff;	}

IC	void PackPosition(NodePosition& Pdest, const Fvector& Psrc)
{
	float sp = 1/m_header.size;
	int px	= iFloor(Psrc.x*sp+EPS_L + .5f);
	int py	= iFloor(65535.f*(Psrc.y-m_header.aabb.min.y)/(m_header.size_y)+EPS_L);
	int pz	= iFloor(Psrc.z*sp+EPS_L + .5f);
	clamp	(px,-32767,32767);	Pdest.x = s16	(px);
	clamp	(py,0,     65535);	Pdest.y = u16	(py);
	clamp	(pz,-32767,32767);	Pdest.z = s16	(pz);
}

IC BOOL	u_InsideNode(const NodeCompressed& N, const NodePosition& P)
{
	return 	((P.x>=N.p0.x)&&(P.x<=N.p1.x))&&	// X inside
			((P.z>=N.p0.z)&&(P.z<=N.p1.z));		// Z inside
}

IC	void UnpackPosition(Fvector& Pdest, const NodePosition& Psrc)
{
	Pdest.x = float(Psrc.x)*m_header.size;
	Pdest.y = (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
	Pdest.z = float(Psrc.z)*m_header.size;
}

IC Fvector tfGetNodeCenter(NodeCompressed *tpNode)
{
	Fvector tP0, tP1;
	UnpackPosition(tP0, tpNode->p0);
	UnpackPosition(tP1, tpNode->p1);
	tP0.add(tP1);
	tP0.mul(.5f);
	return(tP0);
}

IC Fvector tfGetNodeCenter(u32 dwNodeID)
{
	return(tfGetNodeCenter(Node(dwNodeID)));
}

IC float ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, NodeCompressed *tpNode1)
{
	return(tfGetNodeCenter(tpNode0).distance_to(tfGetNodeCenter(tpNode1)));
}

IC float ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, u32 dwNodeID1)
{
	return(ffGetDistanceBetweenNodeCenters(Node(dwNodeID0),Node(dwNodeID1)));
}

IC float ffGetDistanceBetweenNodeCenters(NodeCompressed *tpNode0, u32 dwNodeID1)
{
	return(ffGetDistanceBetweenNodeCenters(tpNode0,Node(dwNodeID1)));
}

IC float ffGetDistanceBetweenNodeCenters(u32 dwNodeID0, NodeCompressed *tpNode1)
{
	return(ffGetDistanceBetweenNodeCenters(Node(dwNodeID0),tpNode1));
}

extern float ffCheckPositionInDirection(u32 dwStartNode, Fvector tStartPosition, Fvector tFinishPosition, float fMaxDistance);

class CNodeThread : public CThread
{
	u32	m_dwStart;
	u32 m_dwEnd;

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

public:
	CNodeThread	(u32 ID, u32 dwStart, u32 dwEnd) : CThread(ID)
	{
		m_dwStart = dwStart;
		m_dwEnd	  = dwEnd;
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		thProgress = 0.0f;
		for (int i = (int)m_dwStart; i<(int)m_dwEnd; thProgress = float(++i - (int)m_dwStart)/dwSize)
			tpaGraph[i].dwNodeID = dwfFindCorrespondingNode(tpaGraph[i].tPoint);
		thProgress = 1.0f;
	}
};

class CGraphThread : public CThread
{
	u32					m_dwStart;
	u32					m_dwEnd;
	u32					m_dwAStarStaticCounter;
	TNode				*m_tpHeap;
	TIndexNode			*m_tpIndexes;
	float				m_fMaxDistance;
	CCriticalSection	*m_tpCriticalSection;
	vector<u32>			tpaNodes;

public:
	CGraphThread(u32 ID, u32 dwStart, u32 dwEnd, float fMaxDistance, CCriticalSection &tCriticalSection) : CThread(ID)
	{
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (m_header.count)*sizeof(TNode);
		m_tpHeap				= (TNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		u32 S2					= (m_header.count)*sizeof(TIndexNode);
		m_tpIndexes				= (TIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		
		m_dwStart				= dwStart;
		m_dwEnd					= dwEnd;
		m_fMaxDistance			= fMaxDistance;
		m_tpCriticalSection		= &tCriticalSection;
	}

	~CGraphThread()
	{
		_FREE(m_tpHeap);
		_FREE(m_tpIndexes);
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		float fDistance;
		u32 N = tpaGraph.size() - 1, M = N + 1, K = N*M/2;
		u32 a = M*m_dwStart - m_dwStart*(m_dwStart + 1)/2, b = M*m_dwEnd - m_dwEnd*(m_dwEnd + 1)/2, c = b - a;
		thProgress = 0.0f;
		for (int i=(int)m_dwStart*1 + 0*194; i<(int)m_dwEnd; i++) {
			SGraphVertex &tCurrentGraphVertex = tpaGraph[i];
			for (int j = (i + 1)*1 + 0*208; j<(int)M; thProgress = (float(M)*i - i*(i + 1)/2 + ++j - i - 1 - a)/c) {
				SGraphVertex &tNeighbourGraphVertex = tpaGraph[j];
				if (tCurrentGraphVertex.tPoint.distance_to(tNeighbourGraphVertex.tPoint) < m_fMaxDistance) {
					//Msg("{%3d,%3d}",i,j);
					try {
						fDistance = ffCheckPositionInDirection(tCurrentGraphVertex.dwNodeID,tCurrentGraphVertex.tPoint,tNeighbourGraphVertex.tPoint,m_fMaxDistance);
						if (fDistance == MAX_VALUE)
							vfFindTheShortestPath(m_tpHeap, m_tpIndexes, m_dwAStarStaticCounter, tCurrentGraphVertex.dwNodeID,tNeighbourGraphVertex.dwNodeID,fDistance,m_fMaxDistance,tCurrentGraphVertex.tPoint,tNeighbourGraphVertex.tPoint,tpaNodes);//,*m_tpCriticalSection,((tCurrentGraphVertex.dwNodeID == 28975) && (tNeighbourGraphVertex.dwNodeID == 6080)));
						if (fDistance < m_fMaxDistance) {
							m_tpCriticalSection->Enter();
							tCurrentGraphVertex.tpaEdges = (SGraphEdge *)xr_realloc(tCurrentGraphVertex.tpaEdges,(++tCurrentGraphVertex.dwNeighbourCount)*sizeof(SGraphEdge));
							tCurrentGraphVertex.tpaEdges[tCurrentGraphVertex.dwNeighbourCount - 1].dwVertexNumber = j;
							tCurrentGraphVertex.tpaEdges[tCurrentGraphVertex.dwNeighbourCount - 1].fPathDistance  = fDistance;
							
							tNeighbourGraphVertex.tpaEdges = (SGraphEdge *)xr_realloc(tNeighbourGraphVertex.tpaEdges,(++tNeighbourGraphVertex.dwNeighbourCount)*sizeof(SGraphEdge));
							tNeighbourGraphVertex.tpaEdges[tNeighbourGraphVertex.dwNeighbourCount - 1].dwVertexNumber = i;
							tNeighbourGraphVertex.tpaEdges[tNeighbourGraphVertex.dwNeighbourCount - 1].fPathDistance  = fDistance;
							m_tpCriticalSection->Leave();
						}
					}
					catch(char *) {
						Msg("%d, %d",i,j);
						R_ASSERT(false);
					}
				}
			}
		}
		thProgress = 1.0f;
	}
};

#endif