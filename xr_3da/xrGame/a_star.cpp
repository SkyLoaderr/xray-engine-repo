////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star.cpp
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "a_star.h"

CAStar::CAStar(u32 dwMaxNodes)
{
	m_tpHeap					= 0;
	m_tpIndexes					= 0;
	m_dwAStarStaticCounter		= 0;
	m_tpaNodes.					clear();
	u32 S1						= (dwMaxNodes + 1)*sizeof(SNode);
	m_tpHeap					= (SNode *)xr_malloc(S1);
	ZeroMemory					(m_tpHeap,S1);
	u32 S2						= max(Level().AI.bfCheckIfMapLoaded() ? Level().AI.Header().count : 0,Level().AI.bfCheckIfGraphLoaded() ? Level().AI.GraphHeader().dwVertexCount + 1 : 0)*sizeof(SIndexNode);
	m_tpIndexes					= (SIndexNode *)xr_malloc(S2);
	ZeroMemory					(m_tpIndexes,S2);
	Msg							("* AI path-finding structures: %d K",(S1 + S2)/(1024));
	m_tpMapPath					= new CAStarSearch<CAIMapShortestPathNode,SAIMapData>		(dwMaxNodes);
	m_tpLCDPath					= new CAStarSearch<CAIMapLCDPathNode,SAIMapDataL>			(dwMaxNodes);
	m_tpEnemyPath				= new CAStarSearch<CAIMapEnemyPathNode,SAIMapDataE>			(dwMaxNodes);
	m_tpEnemyPositionPath		= new CAStarSearch<CAIMapEnemyPositionPathNode,SAIMapDataF> (dwMaxNodes);
	m_tpGraphPath				= new CAStarSearch<CAIGraphShortestPathNode,SAIMapData>     (dwMaxNodes);
}

CAStar::~CAStar()
{
	_FREE(m_tpHeap);
	_FREE(m_tpIndexes);
	_DELETE(m_tpMapPath);
	_DELETE(m_tpLCDPath);
	_DELETE(m_tpEnemyPath);
	_DELETE(m_tpEnemyPositionPath);
	_DELETE(m_tpGraphPath);
}

//////////////////////////////////////////////////////////////////////////
// Optimal paths
//////////////////////////////////////////////////////////////////////////

float CAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode)
{
	return(ffFindMinimalPath(dwStartNode,dwGoalNode,m_tpaNodes));
}

float CAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, vector<u32> &tpaNodes)
{
	SAIMapData			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= &Level().AI;
	m_tpGraphPath->vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,40000.f,fDistance,tpaNodes,false);
	return(fDistance);
}

float CAStar::ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, bool bUseMarks)
{
	SAIMapData			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= &Level().AI;
	m_tpMapPath->vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAStar::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, float fLightWeight, float fCoverWeight, float fDistanceWeight, bool bUseMarks)
{
	SAIMapDataL			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= &Level().AI;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	m_tpLCDPath->vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAStar::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, u32 dwEnemyNode, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	SAIMapDataE			tData;
	float				fDistance;
	tData.dwFinishNode	= dwGoalNode;
	tData.tpAI_Space	= &Level().AI;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	tData.dwEnemyNode	= dwEnemyNode;
	tData.fEnemyDistance = fOptimalEnemyDistance;
	tData.fEnemyView	= fEnemyViewWeight;
	m_tpEnemyPath->vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}

float CAStar::ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::Path& Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight, float fCoverWeight, float fDistanceWeight, float fEnemyViewWeight, bool bUseMarks)
{
	SAIMapDataF			tData;
	float				fDistance;
	tData.tpAI_Space	= &Level().AI;
	tData.fLight		= fLightWeight;
	tData.fCover		= fCoverWeight;
	tData.fDistance		= fDistanceWeight;
	tData.tEnemyPosition = tEnemyPosition;
	tData.fEnemyDistance = fOptimalEnemyDistance;
	tData.fEnemyView	= fEnemyViewWeight;
	m_tpEnemyPositionPath->vfFindOptimalPath(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,tData,dwStartNode,dwGoalNode,1000.f,fDistance,Result.Nodes,bUseMarks);
	return(fDistance);
}