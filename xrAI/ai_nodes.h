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
//#include "xrSyncronize.h"

#include "xrGraph.h"
#include "ai_nodes.h"
#include "ai_a_star_search.h"

extern CStream*					vfs;			// virtual file
extern hdrNODES					m_header;		// m_header
extern BYTE*					m_nodes;		// virtual nodes DATA array
extern NodeCompressed**			m_nodes_ptr;	// pointers to node's data
extern vector<bool>				q_mark_bit;		// temporal usage mark for queries

static float m_fSize2;
static float m_fYSize2;

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

IC Fvector tfGetNodeCenter(u32 tNodeID)
{
	return(tfGetNodeCenter(Node(tNodeID)));
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

	u32 dwfFindCorrespondingNode(Fvector &tLocalPoint)
	{
		NodePosition	P;
		PackPosition	(P,tLocalPoint);
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
				v.set(tLocalPoint.x,P0.y,tLocalPoint.z);	
				PL.intersectRayPoint(v,DUP,v1);
				int dist = iFloor((v1.y - tLocalPoint.y)*(v1.y - tLocalPoint.y));
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
			tpaGraph[i].tNodeID = dwfFindCorrespondingNode(tpaGraph[i].tLocalPoint);
		thProgress = 1.0f;
	}
};

class CAIMapShortestPathNode {
public:
	float		x1;
	float		y1;
	float		z1;
	float		x2;
	float		y2;
	float		z2;
	float		x3;
	float		y3;
	float		z3;
	u32			m_dwLastBestNode;
	typedef		NodeLink* iterator;
	SAIMapData	tData;
	IC CAIMapShortestPathNode(SAIMapData &tAIMapData)
	{
		tData					= tAIMapData;
		m_dwLastBestNode		= u32(-1);
		NodeCompressed &tNode1	= *Node(tData.dwFinishNode);
		x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	}

	IC void begin(u32 dwNode, CAIMapShortestPathNode::iterator &tIterator, CAIMapShortestPathNode::iterator &tEnd)
	{
		tEnd = (tIterator = (NodeLink *)((BYTE *)Node(dwNode) + sizeof(NodeCompressed))) + Node(dwNode)->links;
	}

	IC u32 get_value(CAIMapShortestPathNode::iterator &tIterator)
	{
		return(UnpackLink(*tIterator));
	}

	IC bool bfCheckIfAccessible(u32 dwNode)
	{
		return(q_mark_bit[dwNode]);
	}

	IC float ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
	{
		if (m_dwLastBestNode != dwStartNode) {
			m_dwLastBestNode = dwStartNode;
			NodeCompressed &tNode0 = *Node(dwStartNode), &tNode1 = *Node(dwFinishNode);
			
			x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
			y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
			z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
			
			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			return(_sqrt((float)(m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + m_fYSize2*_sqr(y2 - y1))));
		}
		else {
			NodeCompressed &tNode1 = *Node(dwFinishNode);

			x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
			y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
			z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

			return(_sqrt((float)(m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + m_fYSize2*_sqr(y2 - y1))));
		}
	}

	IC float ffAnticipate(u32 dwStartNode)
	{
		NodeCompressed &tNode0 = *Node(dwStartNode);
		
		x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		return(_sqrt((float)(m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
	}

	IC float ffAnticipate()
	{
		return(_sqrt((float)(m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
	}
};

class CGraphThread : public CThread
{
	u32					m_dwStart;
	u32					m_dwEnd;
	u32					m_dwAStarStaticCounter;
	SNode				*m_tpHeap;
	SNode				**m_tppHeap;
	SIndexNode			*m_tpIndexes;
	float				m_fMaxDistance;
	xrCriticalSection	*m_tpCriticalSection;
	vector<u32>			tpaNodes;
	CAStarSearch<CAIMapShortestPathNode,SAIMapData> m_tpMapPath;

public:
	CGraphThread(u32 ID, u32 dwStart, u32 dwEnd, float fMaxDistance, xrCriticalSection *tpCriticalSection) : CThread(ID)
	{
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (m_header.count + 2)*sizeof(SNode);
		m_tpHeap				= (SNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		u32 S2					= (m_header.count)*sizeof(SIndexNode);
		m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		u32 S3					= (m_header.count)*sizeof(SNode *);
		m_tppHeap				= (SNode **)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		
		m_dwStart				= dwStart;
		m_dwEnd					= dwEnd;
		m_fMaxDistance			= fMaxDistance;
		m_tpCriticalSection		= tpCriticalSection;
	}

	~CGraphThread()
	{
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		u32 dwMaxNodeCount = m_header.count + 2;
		float fDistance;

		u32 N = tpaGraph.size() - 1, M = N + 1, K = N*M/2;
		u32 a = M*m_dwStart - m_dwStart*(m_dwStart + 1)/2, b = M*m_dwEnd - m_dwEnd*(m_dwEnd + 1)/2, c = b - a;
		thProgress = 0.0f;
		for (int i=(int)m_dwStart; i<(int)m_dwEnd; i++) {
			SGraphVertex &tCurrentGraphVertex = tpaGraph[i];
			for (int j = (i + 1); j<(int)M; thProgress = (float(M)*i - i*(i + 1)/2 + ++j - i - 1 - a)/c) {
				SGraphVertex &tNeighbourGraphVertex = tpaGraph[j];
				if (tCurrentGraphVertex.tLocalPoint.distance_to(tNeighbourGraphVertex.tLocalPoint) < m_fMaxDistance) {
					try {
						fDistance = ffCheckPositionInDirection(tCurrentGraphVertex.tNodeID,tCurrentGraphVertex.tLocalPoint,tNeighbourGraphVertex.tLocalPoint,m_fMaxDistance);
						if (fDistance == MAX_VALUE) {
							SAIMapData			tData;
							tData.dwFinishNode	= tNeighbourGraphVertex.tNodeID;
							m_tpMapPath.vfFindOptimalPath(
								m_tppHeap,
								m_tpHeap,
								m_tpIndexes,
								m_dwAStarStaticCounter,
								tData,
								tCurrentGraphVertex.tNodeID,
								tNeighbourGraphVertex.tNodeID,
								fDistance,
								m_fMaxDistance,
								tCurrentGraphVertex.tLocalPoint,
								tNeighbourGraphVertex.tLocalPoint,
								tpaNodes,
								dwMaxNodeCount);
						}
						if (fDistance < m_fMaxDistance) {
							m_tpCriticalSection->Enter();
							int ii = tCurrentGraphVertex.tNeighbourCount;
							int jj = tNeighbourGraphVertex.tNeighbourCount;
							tCurrentGraphVertex.tNeighbourCount++;
							tNeighbourGraphVertex.tNeighbourCount++;
							m_tpCriticalSection->Leave();

							tCurrentGraphVertex.tpaEdges[ii].dwVertexNumber = j;
							tCurrentGraphVertex.tpaEdges[ii].fPathDistance  = fDistance;
							
							tNeighbourGraphVertex.tpaEdges[jj].dwVertexNumber = i;
							tNeighbourGraphVertex.tpaEdges[jj].fPathDistance  = fDistance;
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