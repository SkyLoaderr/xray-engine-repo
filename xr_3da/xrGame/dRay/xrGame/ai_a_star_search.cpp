////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_a_star_search.cpp
//	Created 	: 19.12.2002
//  Modified 	: 19.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* algortihm for finding optimal path from source node to destnation one
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"

//////////////////////////////////////////////////////////////////////////
// Optimal paths
//////////////////////////////////////////////////////////////////////////

float CAI_Space::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, vector<u32> &tpaNodes)
{
	SAIMapData			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	m_tpGraphPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,40000.f,fDistance,tpaNodes,false);
	return(fDistance);
}

float CAI_Space::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, bool bUseMarks)
{
	SAIMapData			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	m_tpMapPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAI_Space::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, float fLightWeight, float fCoverWeight, float fDistanceWeight, bool bUseMarks)
{
	SAIMapDataL			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	m_tpLCDPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAI_Space::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, u32 dwEnemyNode, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	SAIMapDataE			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= this;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	tData.dwEnemyNode	= dwEnemyNode;
	tData.fEnemyDistance = fOptimalEnemyDistance;
	tData.fEnemyView	= fEnemyViewWeight;
	m_tpEnemyPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAI_Space::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	SAIMapDataF			tData;
	float				fDistance;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	tData.tEnemyPosition = tEnemyPosition;
	tData.fEnemyDistance = fOptimalEnemyDistance;
	tData.fEnemyView	= fEnemyViewWeight;
	m_tpEnemyPositionPath.vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

//////////////////////////////////////////////////////////////////////////
// CAIMapShortestPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapShortestPathNode::CAIMapShortestPathNode(SAIMapData &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
}

IC void CAIMapShortestPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapShortestPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
{
	if (m_dwLastBestNode != dwStartNode) {
		m_dwLastBestNode = dwStartNode;
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapShortestPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
}

IC float CAIMapShortestPathNode::ffAnticipate()
{
	return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapLCDPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapLCDPathNode::CAIMapLCDPathNode(SAIMapDataL &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
	float fLight			= (float)(tNode1.light)/255.f;
	m_fSum					= fCover + fLight;
}

IC void CAIMapLCDPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapLCDPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapLCDPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapLCDPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
{
	if (m_dwLastBestNode != dwStartNode) {
		m_dwLastBestNode = dwStartNode;
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);

		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapLCDPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);

	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

IC float CAIMapLCDPathNode::ffAnticipate()
{
	return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapEnemyPathNode::CAIMapEnemyPathNode(SAIMapDataE &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	{
		NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwEnemyNode);
		x4						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y4						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z4						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	}
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
	float fLight			= (float)(tNode1.light)/255.f;
	m_fSum					= fCover + fLight + tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance);
}

IC void CAIMapEnemyPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapEnemyPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapEnemyPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapEnemyPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
{
	if (m_dwLastBestNode != dwStartNode) {
		m_dwLastBestNode = dwStartNode;
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapEnemyPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}		   
		   
IC float CAIMapEnemyPathNode::ffAnticipate()
{		   
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

//////////////////////////////////////////////////////////////////////////
// CAIMapEnemyPositionPathNode
//////////////////////////////////////////////////////////////////////////

CAIMapEnemyPositionPathNode::CAIMapEnemyPositionPathNode(SAIMapDataF &tAIMapData)
{
	tData					= tAIMapData;
	m_dwLastBestNode		= u32(-1);
	{
		x4						= tData.tEnemyPosition.x;
		y4						= tData.tEnemyPosition.y;
		z4						= tData.tEnemyPosition.z;
	}
	NodeCompressed &tNode1	= *tData.tpAI_Space->Node(tData.dwFinishNode);
	x3						= (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
	y3						= (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
	z3						= (float)(tNode1.p1.z) + (float)(tNode1.p0.z);
	float fCover			= 1/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);
	float fLight			= (float)(tNode1.light)/255.f;
	m_fSum					= fCover + fLight + tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance);
}

IC void CAIMapEnemyPositionPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

IC u32 CAIMapEnemyPositionPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

IC bool CAIMapEnemyPositionPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

IC float CAIMapEnemyPositionPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
{
	if (m_dwLastBestNode != dwStartNode) {
		m_dwLastBestNode = dwStartNode;
		NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode), &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x1 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
		y1 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
		z1 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
	else {
		NodeCompressed &tNode1 = *tData.tpAI_Space->Node(dwFinishNode);
		
		x2 = (float)(tNode1.p1.x) + (float)(tNode1.p0.x);
		y2 = (float)(tNode1.p1.y) + (float)(tNode1.p0.y);
		z2 = (float)(tNode1.p1.z) + (float)(tNode1.p0.z);

		float fCover = 1.f/(EPS_L + (float)(tNode1.cover[0])/255.f + (float)(tNode1.cover[1])/255.f + (float)(tNode1.cover[2])/255.f  + (float)(tNode1.cover[3])/255.f);

		float fLight = (float)(tNode1.light)/255.f;
		
		return(tData.fEnemyView*(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x4 - x1) + _sqr(z4 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y4 - y1))) - tData.fEnemyDistance) + fLight*tData.fLight + fCover*tData.fCover + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x1) + _sqr(z2 - z1)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y1))));
	}
}

IC float CAIMapEnemyPositionPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}		   
		   
IC float CAIMapEnemyPositionPathNode::ffAnticipate()
{		   
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

//////////////////////////////////////////////////////////////////////////
// CAIGraphShortestPathNode
//////////////////////////////////////////////////////////////////////////

CAIGraphShortestPathNode::CAIGraphShortestPathNode(SAIMapData &tAIMapData)
{
	tData = tAIMapData;
	m_dwLastBestNode = u32(-1);
}

IC void CAIGraphShortestPathNode::begin(u32 dwNode, CAIGraphTemplateNode::iterator &tIterator, CAIGraphTemplateNode::iterator &tEnd)
{
	tIterator = (AI::SGraphEdge *)((BYTE *)tData.tpAI_Space->m_tpaGraph + tData.tpAI_Space->m_tpaGraph[dwNode].dwEdgeOffset);
	tEnd = tIterator + tData.tpAI_Space->m_tpaGraph[dwNode].dwNeighbourCount;
}

IC bool CAIGraphShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}

IC float CAIGraphShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
{
	m_dwLastBestNode = dwFinishNode;
	return(tIterator->fPathDistance);
}

IC float CAIGraphShortestPathNode::ffAnticipate(u32 dwStartNode)
{
	return(tData.tpAI_Space->m_tpaGraph[dwStartNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[tData.dwFinishNode].tPoint));
}

IC float CAIGraphShortestPathNode::ffAnticipate()
{
	return(tData.tpAI_Space->m_tpaGraph[m_dwLastBestNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[tData.dwFinishNode].tPoint));
}