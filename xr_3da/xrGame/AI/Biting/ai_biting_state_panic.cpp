#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingPanic::CBitingPanic(CAI_Biting *p)
{
	pMonster = p;

	Reset();
	SetHighPriority();
}

void CBitingPanic::Reset()
{
	inherited::Reset();

	m_tEnemy.obj	= 0;

	cur_pos.set		(0.f,0.f,0.f);
	prev_pos		= cur_pos;
	bFacedOpenArea	= false;
	m_dwStayTime	= 0;
}

void CBitingPanic::Init()
{
	inherited::Init();

	// Получить врага
	m_tEnemy = pMonster->m_tEnemy;

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

	// Test
	WRITE_TO_LOG("_ Panic Init _");
}

void CBitingPanic::Run()
{
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;
	
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (m_dwStayTime != 0) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->AI_Path.TravelPath.clear();

		pMonster->r_torso_target.yaw = angle_normalize(pMonster->r_torso_target.yaw + PI);
	} 

//	// нивидимость (for Bloodsucker)
//	if (pMonster->GetPower() > pMonster->m_fPowerThreshold) {
//		if (pMonster->m_tVisibility.Switch(false)) pMonster->ChangePower(pMonster->m_ftrPowerDown);
//	}

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (m_dwStayTime == 0) m_dwStayTime = m_dwCurrentTime;

	pMonster->Path_GetAwayFromPoint(m_tEnemy.obj,m_tEnemy.position, 30, 2000);
	
	if (!bFacedOpenArea) {
			pMonster->MotionMan.m_tAction = ACT_RUN;
	} else {
		// try to rebuild path 
		if (pMonster->AI_Path.TravelPath.size() > 5) {
			pMonster->MotionMan.m_tAction = ACT_RUN;
		} else {
			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;
		}
	}
	
	prev_pos = cur_pos;
}

