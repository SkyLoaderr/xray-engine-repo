#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

#include "../bloodsucker/ai_bloodsucker.h"
#include "../../actor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingPanic::CBitingPanic(CAI_Biting *p, bool invisibility)
{
	pMonster = p;
	m_bInvisibility = invisibility;
	SetPriority(PRIORITY_HIGH);
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
	LOG_EX("panic init");

	inherited::Init();

	// Получить врага
	m_tEnemy = pMonster->m_tEnemy;

	SetInertia(15000);
	pMonster->SetMemoryTime(15000);

}

void CBitingPanic::Run()
{
	if (pMonster->m_tEnemy.obj != m_tEnemy.obj) Init();
	else m_tEnemy = pMonster->m_tEnemy;
	
	cur_pos = pMonster->Position();

	// implementation of 'face the most open area'
	if (!bFacedOpenArea && cur_pos.similar(prev_pos) && (0 != m_dwStayTime) && (m_dwStayTime + 300 < m_dwCurrentTime) && (m_dwStateStartedTime + 3000 < m_dwCurrentTime)) {
		bFacedOpenArea	= true;
		pMonster->enable_movement	(false);
		pMonster->m_body.target.yaw = angle_normalize(pMonster->m_body.target.yaw + PI);
	} 

//#pragma todo("Jim to Jim: fix nesting: Bloodsucker in Biting state")
//	if (m_bInvisibility) {
//		CAI_Bloodsucker *pBS =	dynamic_cast<CAI_Bloodsucker *>(pMonster);
//		CActor			*pA  =  dynamic_cast<CActor*>(Level().CurrentEntity());
//
//		if (pBS && pA && (pA->Position().distance_to(pBS->Position()) < pBS->m_fEffectDist)) {
//			if (pBS->GetPower() > pBS->m_fPowerThreshold) {
//				if (pBS->CMonsterInvisibility::Switch(false)) {
//					pBS->ChangePower(pBS->m_ftrPowerDown);
//					pBS->ActivateEffector(pBS->CMonsterInvisibility::GetInvisibleInterval() / 1000.f);
//				}
//			}
//		}
//	}

	pMonster->SetSound(SND_TYPE_ATTACK, pMonster->_sd->m_dwAttackSndDelay);

	if (!cur_pos.similar(prev_pos)) {
		bFacedOpenArea = false;
		m_dwStayTime = 0;
	} else if (0 == m_dwStayTime) m_dwStayTime = m_dwCurrentTime;

	if (!bFacedOpenArea) {
			pMonster->MotionMan.m_tAction = ACT_RUN;
	} else {
		// try to rebuild path if there is need
		if (!pMonster->CDetailPathManager::completed(pMonster->Position()))
			pMonster->MotionMan.m_tAction = ACT_RUN;
		else  {
			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;
		}
	}
	
	pMonster->SetPathParams(pMonster->level_vertex_id(), pMonster->Position()); 
	//pMonster->Path_GetAwayFromPoint(m_tEnemy.obj,m_tEnemy.position, 30);	

	prev_pos = cur_pos;
}

