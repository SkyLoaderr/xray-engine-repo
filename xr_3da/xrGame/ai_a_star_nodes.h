////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_nodes.h
//	Created 	: 16.12.2002
//  Modified 	: 16.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Node classes for A* algortihm
////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// CAIMapTemplateNode
//////////////////////////////////////////////////////////////////////////

IC void CAIMapTemplateNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapTemplateNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

//////////////////////////////////////////////////////////////////////////
// CAIGraphTemplateNode
//////////////////////////////////////////////////////////////////////////

IC void CAIGraphTemplateNode::begin(u32 dwNode, CAIGraphTemplateNode::iterator &tIterator, CAIGraphTemplateNode::iterator &tEnd)
{
	tIterator = (AI::SGraphEdge *)((BYTE *)tData.tpAI_Space->m_tpaGraph + tData.tpAI_Space->m_tpaGraph[dwNode].dwEdgeOffset);
	tEnd = tIterator + tData.tpAI_Space->m_tpaGraph[dwNode].dwNeighbourCount;
}

//////////////////////////////////////////////////////////////////////////
// CAIMapShortestPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
	
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapLCDPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapLCDPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

	float fLight = (float)(tNode1.light)/255.f;
	
	return(fLight*m_fCriteriaLightWeight + fCover*m_fCriteriaCoverWeight + m_fCriteriaDistanceWeight*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIMapEnemyPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
	float x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	float y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	float z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	float x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	float y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	float z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

	float x3 = 0.f;//tEnemyPosition.x/tData.tpAI_Space->m_fSize;
	float y3 = 0.f;//tEnemyPosition.y/tData.tpAI_Space->m_fYSize;
	float z3 = 0.f;//tEnemyPosition.z/tData.tpAI_Space->m_fSize;

	float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

	float fLight = (float)(tNode1.light)/255.f;
	
	return(m_fCriteriaEnemyViewWeight*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x1) + _sqr(z3 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y1))) - m_fOptimalEnemyDistance) + fLight*m_fCriteriaLightWeight + fCover*m_fCriteriaCoverWeight + m_fCriteriaDistanceWeight*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
}

//////////////////////////////////////////////////////////////////////////
// CAIGraphShortestPathNode
//////////////////////////////////////////////////////////////////////////

IC float CAIGraphShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode)
{
	return(((AI::SGraphEdge *)((BYTE *)tData.tpAI_Space->m_tpaGraph + tData.tpAI_Space->m_tpaGraph[dwStartNode].dwEdgeOffset) + dwFinishNode)->fPathDistance);
}

IC float CAIGraphShortestPathNode::ffAnticipate(u32 dwStartNode, u32 dwFinishNode)
{
	return(tData.tpAI_Space->m_tpaGraph[dwStartNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[dwFinishNode].tPoint));
}
