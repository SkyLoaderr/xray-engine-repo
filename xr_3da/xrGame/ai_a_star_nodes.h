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
	NodeCompressed tNode0 = *Level().AI.Node(dwStartNode), tNode1 = *Level().AI.Node(dwFinishNode);
	
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	return(_sqrt((float)(Level().AI.m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + Level().AI.m_fYSize2*_sqr(y2 - y1))));
}

IC u32 CAIMapShortestPathNode::dwfGetNodeEdgeCount(u32 dwNode)
{
	return(Level().AI.Node(dwNode)->links);
}

IC u32 CAIMapShortestPathNode::dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
{
	return(Level().AI.UnpackLink(((NodeLink *)((BYTE *)Level().AI.Node(dwNode) + sizeof(NodeCompressed)))[dwNeighbourIndex]));
}

IC bool CAIMapShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}

//////////////////////////////////////////////////////////////////////////
// CAIMapLCDPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapLCDPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed tNode0 = *Level().AI.Node(dwStartNode), tNode1 = *Level().AI.Node(dwFinishNode);
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

IC u32 CAIMapLCDPathNode::dwfGetNodeEdgeCount(u32 dwNode)
{
	return(Level().AI.Node(dwNode)->links);
}

IC u32 CAIMapLCDPathNode::dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
{
	return(Level().AI.UnpackLink(((NodeLink *)((BYTE *)Level().AI.Node(dwNode) + sizeof(NodeCompressed)))[dwNeighbourIndex]));
}

IC bool CAIMapLCDPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapEnemyPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed tNode0 = *Level().AI.Node(dwStartNode), tNode1 = *Level().AI.Node(dwFinishNode);
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

IC u32 CAIMapEnemyPathNode::dwfGetNodeEdgeCount(u32 dwNode)
{
	return(Level().AI.Node(dwNode)->links);
}

IC u32 CAIMapEnemyPathNode::dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
{
	return(Level().AI.UnpackLink(((NodeLink *)((BYTE *)Level().AI.Node(dwNode) + sizeof(NodeCompressed)))[dwNeighbourIndex]));
}

IC bool CAIMapEnemyPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}

//////////////////////////////////////////////////////////////////////////
// CAIGraphShortestPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIGraphShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	return(((AI::SGraphEdge *)((BYTE *)Level().AI.m_tpaGraph + Level().AI.m_tpaGraph[dwStartNode].dwEdgeOffset) + dwFinishNode)->fPathDistance);
}

IC u32 CAIGraphShortestPathNode::dwfGetNodeEdgeCount(u32 dwNode)
{
	return(Level().AI.m_tpaGraph[dwNode].dwNeighbourCount);
}

IC u32 CAIGraphShortestPathNode::dwfGetNodeNeighbour(u32 dwNode, u32 dwNeighbourIndex)
{
	return(((AI::SGraphEdge *)((BYTE *)Level().AI.m_tpaGraph + Level().AI.m_tpaGraph[dwNode].dwEdgeOffset) + dwNeighbourIndex)->dwVertexNumber);
}

IC bool CAIGraphShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}