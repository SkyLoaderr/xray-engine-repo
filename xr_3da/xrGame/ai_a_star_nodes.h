////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_nodes.h
//	Created 	: 16.12.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Node classes for A* algortihm
////////////////////////////////////////////////////////////////////////////

#include "ai_space.h"

#pragma once

class CAIMapShortestPathNode {
public:
	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
	{
		float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		return(_sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + 0*fYSize2*_sqr(y2 - y1))));
	}

	IC u32		dwfGetNodeEdgeCount(u32 dwNode)
	{
		return(AI.Node(dwNode)->links);
	}

	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
	{
		return(AI.UnpackLink(((NodeLink *)((BYTE *)AI.Node(dwNode) + sizeof(NodeCompressed)))[dwNeighbourIndex]));
	}
};

class CAIMapLCDPathNode {
private:
	float	
		m_fCriteriaLightWeight,
		m_fCriteriaCoverWeight,
		m_fCriteriaDistanceWeight;

public:
	CAIMapLCDPathNode()
	{
		m_fCriteriaLightWeight = 5.f;
		m_fCriteriaCoverWeight = 10.f;
		m_fCriteriaDistanceWeight 40.f;
	}

	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
	{
		float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(fLight*m_fCriteriaLightWeight + fCover*m_fCriteriaCoverWeight + m_fCriteriaDistanceWeight*_sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + 0*fYSize2*_sqr(y2 - y1))));
	}

	IC u32		dwfGetNodeEdgeCount(u32 dwNode)
	{
		return(AI.Node(dwNode)->links);
	}

	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
	{
		return(AI.UnpackLink(((NodeLink *)((BYTE *)AI.Node(dwNode) + sizeof(NodeCompressed)))[dwNeighbourIndex]));
	}
};

class CAIMapEnemyRelativePathNode {
private:
	float	
		m_fCriteriaEnemyViewWeight,
		m_fCriteriaEnemyViewWeight,
		m_fCriteriaCoverWeight,
		m_fCriteriaDistanceWeight;

public:
	CAIMapEnemyRelativePathNode()
	{
		m_fCriteriaEnemyViewWeight = 5.f;
		m_fCriteriaCoverWeight = 10.f;
		m_fCriteriaDistanceWeight = 40.f;
		m_fCriteriaEnemyViewWeight = 100.f;
	}

	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
	{
		float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float x3 = tEnemyPosition.x/fSize;
		float y3 = tEnemyPosition.y/fYSize;
		float z3 = tEnemyPosition.z/fSize;

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(m_fCriteriaEnemyViewWeight*(_sqrt((float)(fSize2*(_sqr(x3 - x1) + _sqr(z3 - z1)) + fYSize2*_sqr(y3 - y1))) - fOptimalEnemyDistance) + fLight*fCriteriaLightWeight + fCover*fCriteriaCoverWeight + fCriteriaDistanceWeight*(float)sqrt((float)(fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + fYSize2*_sqr(y2 - y1))));
	}

	IC u32		dwfGetNodeEdgeCount(u32 dwNode)
	{
		return(AI.Node(dwNode)->links);
	}

	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
	{
		return(AI.UnpackLink(((NodeLink *)((BYTE *)AI.Node(dwNode) + sizeof(NodeCompressed)))[dwNeighbourIndex]));
	}
};

class CAIGraphShortestPathNode {
public:
	IC float	ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
	{
		return(((SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[dwStartNode].dwEdgeOffset) + dwFinishNode)->fPathDistance);
	}

	IC u32		dwfGetNodeEdgeCount(u32 dwNode)
	{
		return(m_tpaGraph[dwStartNode].dwNeighbourCount);
	}

	IC u32		dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
	{
		return(((SGraphEdge *)((BYTE *)m_tpaGraph + m_tpaGraph[i].dwEdgeOffset) + j)->dwVertexNumber);
	}
};

