////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_nodes.h
//	Created 	: 16.12.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Node classes for A* algortihm
////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// CAIMapShortestPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed &tNode0 = *Level().AI.Node(dwStartNode), &tNode1 = *Level().AI.Node(dwFinishNode);
	
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	return(_sqrt((float)(Level().AI.m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + Level().AI.m_fYSize2*_sqr(y2 - y1))));
}

IC void CAIMapShortestPathNode::begin(u32 dwNode, CAIMapShortestPathNode::iterator &tIterator, CAIMapShortestPathNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)Level().AI.Node(dwNode) + sizeof(NodeCompressed))) + Level().AI.Node(dwNode)->links;
}

IC u32 CAIMapShortestPathNode::get_value(CAIMapShortestPathNode::iterator &tIterator)
{
	return(Level().AI.UnpackLink(*tIterator));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapLCDPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapLCDPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed &tNode0 = *Level().AI.Node(dwStartNode), &tNode1 = *Level().AI.Node(dwFinishNode);
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

	float fLight = (float)(tNode1.light)/255.f;
	
	return(fLight*m_fCriteriaLightWeight + fCover*m_fCriteriaCoverWeight + m_fCriteriaDistanceWeight*_sqrt((float)(Level().AI.m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + Level().AI.m_fYSize2*_sqr(y2 - y1))));
}

IC void CAIMapLCDPathNode::begin(u32 dwNode, CAIMapLCDPathNode::iterator &tIterator, CAIMapLCDPathNode::iterator &tEnd)
{
	NodeCompressed *tpNode = Level().AI.Node(dwNode);
	tIterator = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
	tEnd = tIterator + tpNode->links;
}

IC u32 CAIMapLCDPathNode::get_value(CAIMapLCDPathNode::iterator &tIterator)
{
	return(Level().AI.UnpackLink(*tIterator));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapEnemyPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed &tNode0 = *Level().AI.Node(dwStartNode), &tNode1 = *Level().AI.Node(dwFinishNode);
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float x3 = 0.f;//tEnemyPosition.x/Level().AI.m_fSize;
	float y3 = 0.f;//tEnemyPosition.y/Level().AI.m_fYSize;
	float z3 = 0.f;//tEnemyPosition.z/Level().AI.m_fSize;

	float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

	float fLight = (float)(tNode1.light)/255.f;
	
	return(m_fCriteriaEnemyViewWeight*(_sqrt((float)(Level().AI.m_fSize2*(_sqr(x3 - x1) + _sqr(z3 - z1)) + Level().AI.m_fYSize2*_sqr(y3 - y1))) - m_fOptimalEnemyDistance) + fLight*m_fCriteriaLightWeight + fCover*m_fCriteriaCoverWeight + m_fCriteriaDistanceWeight*(float)sqrt((float)(Level().AI.m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + Level().AI.m_fYSize2*_sqr(y2 - y1))));
}

IC void CAIMapEnemyPathNode::begin(u32 dwNode, CAIMapEnemyPathNode::iterator &tIterator, CAIMapEnemyPathNode::iterator &tEnd)
{
	NodeCompressed *tpNode = Level().AI.Node(dwNode);
	tIterator = (NodeLink *)((BYTE *)tpNode + sizeof(NodeCompressed));
	tEnd = tIterator + tpNode->links;
}

//////////////////////////////////////////////////////////////////////////
// CAIGraphShortestPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIGraphShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	return(((AI::SGraphEdge *)((BYTE *)Level().AI.m_tpaGraph + Level().AI.m_tpaGraph[dwStartNode].dwEdgeOffset) + dwFinishNode)->fPathDistance);
}

IC float CAIGraphShortestPathNode::ffAnticipate(u32 dwStartNode, u32 dwFinishNode)
{
	return(Level().AI.m_tpaGraph[dwStartNode].tPoint.distance_to(Level().AI.m_tpaGraph[dwFinishNode].tPoint));
}

IC void CAIGraphShortestPathNode::begin(u32 dwNode, CAIGraphShortestPathNode::iterator &tIterator, CAIGraphShortestPathNode::iterator &tEnd)
{
	tIterator = (AI::SGraphEdge *)((BYTE *)Level().AI.m_tpaGraph + Level().AI.m_tpaGraph[dwNode].dwEdgeOffset);
	tEnd = tIterator + Level().AI.m_tpaGraph[dwNode].dwNeighbourCount;
}