#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraAttack implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CBloodsuckerRest::CBloodsuckerRest(CAI_Bloodsucker *p)  
{
	pMonster = p;
	Reset();
}


void CBloodsuckerRest::Reset()
{
	IState::Reset();
	m_tAction			= ACTION_RUN;
}

void CBloodsuckerRest::Init()
{
	IState::Init();

	WRITE_TO_LOG("_ Blood State Init _");
}

void CBloodsuckerRest::Run()
{
	// Выполнение состояния
	switch (m_tAction) {	
		case ACTION_RUN:

			float newYaw,tP;
			pMonster->Direction().getHP(newYaw,tP);

			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw + PI_DIV_2),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW); //, STOP_YAW | STOP_ANIM | STOP_TIME);
			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw - PI_DIV_2),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW);
			pMonster->Motion.m_tSeq.Switch();
			
			WRITE_TO_LOG("_ Switch _");

			break;
	}
}

bool CBloodsuckerRest::CheckCompletion()
{	
	return false;
}

