////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_a_star.h
//	Created 	: 21.01.2003
//  Modified 	: 21.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life A* realization
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "a_star_template.h"
#include "a_star_interfaces.h"

class CSE_ALifeAStar {
public:
	SNode													*m_tpHeap;
	SIndexNode												*m_tpIndexes;
	u32														m_dwAStarStaticCounter;
	AI::DWORD_VECTOR										m_tpaNodes;
	SAIMapDataG												m_tAIGraphData;
	CAStarSearch<CAIGraphShortestPathNode,SAIMapDataG>		*m_tpGraphPath;

											CSE_ALifeAStar()
	{
		m_tpHeap							= 0;
		m_tpIndexes							= 0;
		m_dwAStarStaticCounter				= 0;
		m_tpaNodes.clear					();
	};

	virtual									~CSE_ALifeAStar()
	{
		xr_free								(m_tpHeap);
		xr_free								(m_tpIndexes);
		xr_delete							(m_tpGraphPath);
	}

	virtual void							Init()
	{
		u32 S1								= (getAI().GraphHeader().dwVertexCount + 2)*sizeof(SNode);
		m_tpHeap							= (SNode *)xr_malloc(S1);
		ZeroMemory							(m_tpHeap,S1);
		u32 S2								= getAI().GraphHeader().dwVertexCount*sizeof(SIndexNode);
		m_tpIndexes							= (SIndexNode *)xr_malloc(S2);
		ZeroMemory							(m_tpIndexes,S2);
		Msg									("* ALife path-finding structures: %d K",(S1 + S2)/(1024));
		m_tpGraphPath						= xr_new<CAStarSearch<CAIGraphShortestPathNode,SAIMapDataG> >   (getAI().GraphHeader().dwVertexCount + 2);
		m_tAIGraphData.tpGraph				= &(getAI());
	}

	float									ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode)
	{
		return								(ffFindMinimalPath(dwStartNode, dwGoalNode, m_tpaNodes));
	}

	float									ffFindMinimalPath(u32 dwStartNode, u32 dwGoalNode, AI::DWORD_VECTOR &tpaNodes)
	{
		float								fDistance;
		m_tAIGraphData.dwFinishNode			= dwGoalNode;
		m_tpGraphPath->vfFindOptimalPath	(m_tpHeap,m_tpIndexes,m_dwAStarStaticCounter,m_tAIGraphData,dwStartNode,dwGoalNode,40000.f,fDistance,tpaNodes,false);
		return								(fDistance);
	}
};