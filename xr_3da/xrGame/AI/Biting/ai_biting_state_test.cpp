#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingTest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingTest::CBitingTest(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_NORMAL);
}

void CBitingTest::Init()
{
	inherited::Init();

	last_time	= 0;
	vertex_id	= 0;
	start_vertex_id = pMonster->level_vertex_id();
}

void CBitingTest::Run()
{

	bool bNeedRebuild = false;

	xr_vector<u32> nodes;
	float radius = 20.f;

	if (pMonster->CMovementManager::path_completed() || (!pMonster->CDetailPathManager::path().empty() && (pMonster->CDetailPathManager::path().size() < pMonster->CDetailPathManager::curr_travel_point_index() + 5 ))) {
		bNeedRebuild = true;
	} else {
		DO_IN_TIME_INTERVAL_BEGIN(last_time, 5000);
			bNeedRebuild = true;
		DO_IN_TIME_INTERVAL_END();
	}
	
	
	if (bNeedRebuild) {
		// get new vertex_id here
//		while (true) {
//			get_node_in_dir();
//
//			ai().level_graph().check_position_in_direction(start_vertex_id, ) 
//
//
//		}

		ai().graph_engine().search( ai().level_graph(), start_vertex_id, start_vertex_id, &nodes, CGraphEngine::CFlooder(radius));
		vertex_id = nodes[::Random.randI(nodes.size())];
	}

	pMonster->MotionMan.m_tAction = ACT_RUN;
	pMonster->SetPathParams(CMovementManager::ePathTypeLevelPath, vertex_id, ai().level_graph().vertex_position(vertex_id));

}
