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
}

void CBitingTest::Run()
{
	u32 vertex_id;

	if (pMonster->CDetailPathManager::completed(pMonster->Position())) {
		xr_vector<u32> nodes;
		float radius = 20.0f;

		ai().graph_engine().search( ai().level_graph(), pMonster->level_vertex_id(), pMonster->level_vertex_id(), &nodes, CGraphEngine::CFlooder(radius));
		
		vertex_id= nodes[::Random.randI(nodes.size())];
	} else 
		vertex_id = pMonster->CDetailPathManager::path()[pMonster->CDetailPathManager::path().size()-1].vertex_id;
	
	pMonster->SetPathParams(CMovementManager::ePathTypeLevelPath, vertex_id, ai().level_graph().vertex_position(vertex_id));
	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;
}
