////////////////////////////////////////////////////////////////////////////
//	Module 		: location_selector.cpp
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Location selector
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "location_selector.h"
#include "ai_space.h"
#include "entity.h"

CLocationSelector::CLocationSelector		()
{
}

CLocationSelector::~CLocationSelector		()
{
}

void CLocationSelector::init_evaluator		(PathManagers::CAbstractNodeEvaluator *node_evaluator)
{
	CEntity								*l_tpEntity = dynamic_cast<CEntity*>(this);
	VERIFY								(l_tpEntity);
	CSquad								&Squad = Level().Teams[l_tpEntity->g_Team()].Squads[l_tpEntity->g_Squad()];
	CEntity								*Leader = Squad.Leader;
	if (Leader->g_Health() <= 0)
		Leader							= l_tpEntity;
	R_ASSERT							(Leader);
	node_evaluator->m_tHitDir			= m_tHitDirection;
	node_evaluator->m_dwHitTime			= m_dwHitTime;
	node_evaluator->m_dwCurTime			= m_dwCurrentUpdate;
	node_evaluator->m_tMe				= l_tpEntity;
	node_evaluator->m_tpMyNode			= m_tpLevelVertex;
	node_evaluator->m_tMyPosition		= Position();
	node_evaluator->m_tEnemy			= m_tEnemy.Enemy;
	node_evaluator->m_tEnemyPosition	= m_tEnemy.Enemy->Position();
	node_evaluator->m_dwEnemyNode		= m_dwLevelVertexID;
	R_ASSERT2							(ai().level_graph().valid_vertex_id(m_dwLevelVertexID), "Invalid enemy vertex");
	node_evaluator->m_tpEnemyNode		= m_tEnemy.Enemy->m_tpLevelVertex;
	node_evaluator->m_taMembers			= &(Squad.Groups[g_Group()].Members);
	node_evaluator->m_dwStartNode		= m_dwLevelVertexID;
	node_evaluator->m_tStartPosition	= Position();
}

void CLocationSelector::select_location		(CAbstractNodeEvaluator *node_evaluator)
{
	CSquad						&Squad = Level().Teams[g_Team()].Squads[g_Squad()];
	CEntity						*Leader = Squad.Leader;
	if (Leader->g_Health() <= 0)
		Leader					= this;
	R_ASSERT					(Leader);

	Device.Statistic.AI_Range.Begin();
	
	if ((!m_dwLastLocationSelectionTime) || (m_detail_path.empty()) || (int(m_detail_cur_point_index) > int(m_detail_path.size()) - 4) || (speed() < EPS_L) || ((tNodeEvaluator.m_dwCurTime - m_dwLastRangeSearch > MIN_RANGE_SEARCH_TIME_INTERVAL))) {
		
		m_dwLastLocationSelectionTime = tNodeEvaluator.m_dwCurTime;
		
		Squad.Groups[g_Group()].GetAliveMemberInfo(*node_evaluator.m_taMemberPositions, tNodeEvaluator.m_taMemberNodes, tNodeEvaluator.m_taDestMemberPositions, tNodeEvaluator.m_taDestMemberNodes, this);
		
		ai().graph_search_engine().build_path(ai().level_graph(),m_tLevelVertexID,m_tLevelVertexID,0,PathManagers::);
		tNodeEvaluator.vfShallowGraphSearch(ai().q_mark_bit);
	
		if ((m_level_dest_node != tNodeEvaluator.m_dwBestNode) && (tNodeEvaluator.m_fBestCost < fOldCost - 0.f)){
			m_level_dest_node		= tNodeEvaluator.m_dwBestNode;
			if (!m_level_dest_node) {
				Msg("! Invalid vertex Evaluator vertex");
			}
			m_level_path.clear		();
			m_tPathState			= ePathStateBuildNodePath;
			m_bIfSearchFailed		= false;
		} 
		else
			m_bIfSearchFailed		= true;
	}

	Device.Statistic.AI_Range.End();
}