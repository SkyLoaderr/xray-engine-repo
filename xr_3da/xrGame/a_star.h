////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star.h
//	Created 	: 21.03.2002
//  Modified 	: 26.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A* class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "a_star_template.h"
#include "a_star_interfaces.h"

#define	DEFAULT_LIGHT_WEIGHT								  5.f 
#define	DEFAULT_COVER_WEIGHT								 10.f 
#define	DEFAULT_DISTANCE_WEIGHT								 40.f
#define	DEFAULT_ENEMY_VIEW_WEIGHT							100.f

class CAStar {
private:
	SAIMapData												m_tAIMapData;
	SAIMapDataL												m_tAIMapDataL;
	SAIMapDataE												m_tAIMapDataE;
	SAIMapDataF												m_tAIMapDataF;
	SAIMapDataG												m_tAIGraphData;
public:
	
	SNode													*m_tpHeap;
	SIndexNode												*m_tpIndexes;
	u32														m_dwAStarStaticCounter;
	DWORD_VECTOR											m_tpaNodes;

	CAStarSearch<CAIMapShortestPathNode,SAIMapData>			*m_tpMapPath;
	CAStarSearch<CAIMapLCDPathNode,SAIMapDataL>				*m_tpLCDPath;
	CAStarSearch<CAIMapEnemyPathNode,SAIMapDataE>			*m_tpEnemyPath;
	CAStarSearch<CAIMapEnemyPositionPathNode,SAIMapDataF>   *m_tpEnemyPositionPath;
	CAStarSearch<CAIGraphShortestPathNode,SAIMapDataG>      *m_tpGraphPath;
					CAStar(u32 dwMaxNodes);
	virtual			~CAStar();
			float	ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode);
			float	ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, DWORD_VECTOR	&tpaNodes);
			float	ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath &Result, bool bUseMarks = false);
			float	ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath &Result, float fLightWeight = 0*DEFAULT_LIGHT_WEIGHT, float fCoverWeight = 0*DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT, bool bUseMarks = false);
			float	ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath &Result, u32 dwEnemyNode, float fOptimalEnemyDistance, float fLightWeight = DEFAULT_LIGHT_WEIGHT, float fCoverWeight = DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT, float fEnemyViewWeight = DEFAULT_ENEMY_VIEW_WEIGHT, bool bUseMarks = false);
			float	ffFindOptimalPath(u32 dwStartNode, u32 dwGoalNode, AI::NodePath &Result, Fvector tEnemyPosition, float fOptimalEnemyDistance, float fLightWeight = DEFAULT_LIGHT_WEIGHT, float fCoverWeight = DEFAULT_COVER_WEIGHT, float fDistanceWeight = DEFAULT_DISTANCE_WEIGHT, float fEnemyViewWeight = DEFAULT_ENEMY_VIEW_WEIGHT, bool bUseMarks = false);
};
