#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBitingDetour::CBitingDetour(CAI_Biting *p)
{
	pMonster = p;
	Reset();
	SetNormalPriority();
}

void CBitingDetour::Reset()
{
	inherited::Reset();
	m_tEnemy.obj		= 0;
}

void CBitingDetour::Init()
{
	inherited::Init();

	if (!pMonster->GetEnemy(m_tEnemy)) R_ASSERT(false);

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	WRITE_TO_LOG(" DETOUR init!");
}

void CBitingDetour::Run()
{
	WRITE_TO_LOG("--- DETOUR ---");

	VisionElem tempEnemy;
	if (pMonster->GetEnemy(tempEnemy)) m_tEnemy = tempEnemy;

	pMonster->vfUpdateDetourPoint	();
	pMonster->AI_Path.DestNode		= getAI().m_tpaGraph[pMonster->m_tNextGP].tNodeID;
	pMonster->Path_CoverFromPoint	(m_tEnemy.obj,m_tEnemy.obj->Position(),2000);

	pMonster->MotionMan.m_tAction = ACT_WALK_FWD;

	pMonster->SetSound(pMonster->SND_TYPE_IDLE, pMonster->m_dwIdleSndDelay, false);
}


