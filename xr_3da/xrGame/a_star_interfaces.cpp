////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star_interfaces.cpp
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : Interfaces for A* algortihm
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "a_star_interfaces.h"

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

void CAIMapShortestPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

u32 CAIMapShortestPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

bool CAIMapShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

float CAIMapShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
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

float CAIMapShortestPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x3 - x2) + _sqr(z3 - z2)) + tData.tpAI_Space->m_fYSize2*_sqr(y3 - y2))));
}

float CAIMapShortestPathNode::ffAnticipate()
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

void CAIMapLCDPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

u32 CAIMapLCDPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

bool CAIMapLCDPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

float CAIMapLCDPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
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

float CAIMapLCDPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);

	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*_sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}

float CAIMapLCDPathNode::ffAnticipate()
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

void CAIMapEnemyPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

u32 CAIMapEnemyPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

bool CAIMapEnemyPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

float CAIMapEnemyPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
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

float CAIMapEnemyPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}		   
		   
float CAIMapEnemyPathNode::ffAnticipate()
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

void CAIMapEnemyPositionPathNode::begin(u32 dwNode, CAIMapTemplateNode::iterator &tIterator, CAIMapTemplateNode::iterator &tEnd)
{
	tEnd = (tIterator = (NodeLink *)((BYTE *)tData.tpAI_Space->Node(dwNode) + sizeof(NodeCompressed))) + tData.tpAI_Space->Node(dwNode)->links;
}

u32 CAIMapEnemyPositionPathNode::get_value(CAIMapTemplateNode::iterator &tIterator)
{
	return(tData.tpAI_Space->UnpackLink(*tIterator));
}

bool CAIMapEnemyPositionPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(tData.tpAI_Space->q_mark_bit[dwNode]);
}

float CAIMapEnemyPositionPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
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

float CAIMapEnemyPositionPathNode::ffAnticipate(u32 dwStartNode)
{
	NodeCompressed &tNode0 = *tData.tpAI_Space->Node(dwStartNode);
	
	x2 = (float)(tNode0.p1.x) + (float)(tNode0.p0.x);
	y2 = (float)(tNode0.p1.y) + (float)(tNode0.p0.y);
	z2 = (float)(tNode0.p1.z) + (float)(tNode0.p0.z);
	
	return(m_fSum + tData.fDistance*(float)sqrt((float)(tData.tpAI_Space->m_fSize2*(_sqr(x2 - x3) + _sqr(z2 - z3)) + tData.tpAI_Space->m_fYSize2*_sqr(y2 - y3))));
}		   
		   
float CAIMapEnemyPositionPathNode::ffAnticipate()
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

void CAIGraphShortestPathNode::begin(u32 dwNode, CAIGraphTemplateNode::iterator &tIterator, CAIGraphTemplateNode::iterator &tEnd)
{
	tIterator = (AI::SGraphEdge *)((BYTE *)tData.tpAI_Space->m_tpaGraph + tData.tpAI_Space->m_tpaGraph[dwNode].dwEdgeOffset);
	tEnd = tIterator + tData.tpAI_Space->m_tpaGraph[dwNode].dwNeighbourCount;
}

bool CAIGraphShortestPathNode::bfCheckIfAccessible(u32 dwNode)
{
	return(true);
}

float CAIGraphShortestPathNode::ffEvaluate(u32 dwStartNode, u32 dwFinishNode, iterator &tIterator)
{
	m_dwLastBestNode = dwFinishNode;
	return(tIterator->fPathDistance);
}

float CAIGraphShortestPathNode::ffAnticipate(u32 dwStartNode)
{
	return(tData.tpAI_Space->m_tpaGraph[dwStartNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[tData.dwFinishNode].tPoint));
}

float CAIGraphShortestPathNode::ffAnticipate()
{
	return(tData.tpAI_Space->m_tpaGraph[m_dwLastBestNode].tPoint.distance_to(tData.tpAI_Space->m_tpaGraph[tData.dwFinishNode].tPoint));
}