////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_nodes.h
//	Created 	: 09.12.2002
//  Modified 	: 09.12.2002
//	Author		: Dmitriy Iassenev
//	Description : AI nodes structures and functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "compiler.h"
#include "xrThread.h"

#include "xrGraph.h"
#include "level_graph.h"
#include "ai_a_star_search.h"


class CNodeThread : public CThread
{
	u32				m_dwStart;
	u32				m_dwEnd;
	CAI_Map			*m_tpAI_Map;

public:
	CNodeThread	(u32 ID, u32 dwStart, u32 dwEnd, CAI_Map *tpAI_Map) : CThread(ID)
	{
		m_dwStart  = dwStart;
		m_dwEnd	   = dwEnd;
		m_tpAI_Map = tpAI_Map;
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		thProgress = 0.0f;
		for (int i = (int)m_dwStart; i<(int)m_dwEnd; thProgress = float(++i - (int)m_dwStart)/dwSize) {
			tpaGraph[i].tNodeID = m_tpAI_Map->dwfFindCorrespondingNode(tpaGraph[i].tLocalPoint);
			if ((int(tpaGraph[i].tNodeID) > 0) && !m_tpAI_Map->bfInsideNode(m_tpAI_Map->Node(tpaGraph[i].tNodeID),tpaGraph[i].tLocalPoint,true))
				tpaGraph[i].tNodeID = u32(-1);
		}
		thProgress = 1.0f;
	}
};

class CAIMapShortestPathNode {
public:
	typedef		CAI_Map::const_iterator const_iterator;
	int			x1;
	float		y1;
	int			z1;
	int			x2;
	float		y2;
	int			z2;
	int			x3;
	float		y3;
	int			z3;
	SAIMapData	tData;
	float		m_fSize2;
	float		m_fYSize2;
	NodeCompressed	*m_tpNode;
	float		m_sqr_distance_xz;


	IC CAIMapShortestPathNode(const SAIMapData &tAIMapData)
	{
		tData					= tAIMapData;
		const NodeCompressed	&tNode1	= *tData.m_tpAI_Map->Node(tData.dwFinishNode);
		tData.m_tpAI_Map->unpack_xz(tNode1,x3,z3);
		m_sqr_distance_xz		= _sqr(tData.m_tpAI_Map->m_header.size);
		y3						= (float)(tNode1.p.y);
		m_fSize2				= tData.m_tpAI_Map->m_fSize2;
		m_fYSize2				= tData.m_tpAI_Map->m_fYSize2;
	}

	IC void begin(u32 dwNode, const_iterator &tIterator, const_iterator &tEnd)
	{
		tData.m_tpAI_Map->begin	(dwNode,tIterator,tEnd);
		m_tpNode				= tData.m_tpAI_Map->Node(dwNode);
		const NodeCompressed	&tNode0	= *m_tpNode;

//		tData.m_tpAI_Map->unpack_xz(tNode0,x1,z1);
		y1						= (float)(tNode0.p.y);
	}

	IC u32 get_value(const_iterator &tIterator)
	{
		return(m_tpNode->get_link(tIterator));
	}

	IC bool bfCheckIfAccessible(u32 dwNode)
	{
		return(tData.m_tpAI_Map->q_mark_bit[dwNode]);
	}

	IC float ffEvaluate(u32 dwStartNode, u32 dwFinishNode, const_iterator &tIterator)
	{
		const NodeCompressed	&tNode1 = *tData.m_tpAI_Map->Node(dwFinishNode);

//		tData.m_tpAI_Map->unpack_xz(tNode1,x2,z2);
		y2						= (float)(tNode1.p.y);

		return					(_sqrt(m_fYSize2*_sqr(y2 - y1) + m_sqr_distance_xz));
	}

	IC float ffAnticipate(u32 dwStartNode)
	{
		const NodeCompressed	&tNode0 = *tData.m_tpAI_Map->Node(dwStartNode);
		
		tData.m_tpAI_Map->unpack_xz(tNode0,x2,z2);
		y2						= (float)(tNode0.p.y);

		return					(_sqrt((float)(m_fSize2*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
	}

	IC float ffAnticipate()
	{
		return					(_sqrt((float)(m_fSize2*float(_sqr(x3 - x2) + _sqr(z3 - z2)) + m_fYSize2*_sqr(y3 - y2))));
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
	xr_vector<u32>			tpaNodes;
	CAStarSearch<CAIMapShortestPathNode,SAIMapData> m_tpMapPath;
	const CAI_Map		*m_tpAI_Map;

public:
	CGraphThread(u32 ID, u32 dwStart, u32 dwEnd, float fMaxDistance, xrCriticalSection *tpCriticalSection, const CAI_Map *tAI_Map) : CThread(ID)
	{
		m_dwStart				= dwStart;
		m_dwEnd					= dwEnd;
		m_fMaxDistance			= fMaxDistance;
		m_tpCriticalSection		= tpCriticalSection;
		m_tpAI_Map				= tAI_Map;
		
		m_dwAStarStaticCounter	= 0;
		u32 S1					= (m_tpAI_Map->m_header.count + 2)*sizeof(SNode);
		m_tpHeap				= (SNode *)xr_malloc(S1);
		ZeroMemory				(m_tpHeap,S1);
		u32 S2					= (m_tpAI_Map->m_header.count)*sizeof(SIndexNode);
		m_tpIndexes				= (SIndexNode *)xr_malloc(S2);
		ZeroMemory				(m_tpIndexes,S2);
		u32 S3					= (m_tpAI_Map->m_header.count)*sizeof(SNode *);
		m_tppHeap				= (SNode **)xr_malloc(S3);
		ZeroMemory				(m_tppHeap,S3);
	}

	~CGraphThread()
	{
		xr_free(m_tpHeap);
		xr_free(m_tpIndexes);
		xr_free(m_tppHeap);
	}
	
	virtual void Execute()
	{
		u32 dwSize = m_dwEnd - m_dwStart + 1;
		u32 dwMaxNodeCount = m_tpAI_Map->m_header.count + 2;
		float fDistance;

		u32 N = tpaGraph.size() - 1, M = N + 1, K = N*M/2;
		u32 a = M*m_dwStart - m_dwStart*(m_dwStart + 1)/2, b = M*m_dwEnd - m_dwEnd*(m_dwEnd + 1)/2, c = b - a;
		thProgress = 0.0f;
		for (int i=(int)m_dwStart; i<(int)m_dwEnd; i++) {
			SDynamicGraphVertex &tCurrentGraphVertex = tpaGraph[i];
			for (int j = (i + 1); j<(int)M; thProgress = (float(M)*i - i*(i + 1)/2 + ++j - i - 1 - a)/c) {
				SDynamicGraphVertex &tNeighbourGraphVertex = tpaGraph[j];
				float fStraightDistance = tCurrentGraphVertex.tLocalPoint.distance_to(tNeighbourGraphVertex.tLocalPoint);
				if (fStraightDistance < m_fMaxDistance) {
					try {
						if (fStraightDistance < 200.f)
							fDistance = m_tpAI_Map->ffCheckPositionInDirection(tCurrentGraphVertex.tNodeID,tCurrentGraphVertex.tLocalPoint,tNeighbourGraphVertex.tLocalPoint,m_fMaxDistance);
						else
							fDistance = MAX_VALUE;
//						fDistance = m_tpAI_Map->ffCheckPositionInDirection(tCurrentGraphVertex.tNodeID,tCurrentGraphVertex.tLocalPoint,tNeighbourGraphVertex.tLocalPoint,m_fMaxDistance);
						if (fDistance == MAX_VALUE) {
							SAIMapData			tData;
							tData.dwFinishNode	= tNeighbourGraphVertex.tNodeID;
							tData.m_tpAI_Map	= m_tpAI_Map;
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