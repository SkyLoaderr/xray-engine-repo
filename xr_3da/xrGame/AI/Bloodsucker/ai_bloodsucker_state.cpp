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

	//Look leftside/rightside





	WRITE_TO_LOG("_ Blood State Init _");
}

void CBloodsuckerRest::Run()
{
	// Выполнение состояния
	switch (m_tAction) {	
		case ACTION_RUN:

//			float newYaw;
//			newYaw = pMonster->r_torso_current.yaw;
//
//			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw + PI_DIV_4),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_ANIM_END);
//			pMonster->Motion.m_tSeq.Add(eMotionStandIdle,0,pMonster->m_ftrWalkTurningSpeed,angle_normalize(newYaw - PI_DIV_2),0,MASK_ANIM | MASK_SPEED | MASK_R_SPEED | MASK_YAW, STOP_AT_TURNED);
//			pMonster->Motion.m_tSeq.Switch();
//			
//			WRITE_TO_LOG("_ Switch _");

			break;
	}
}

bool CBloodsuckerRest::CheckCompletion()
{	
	return false;
}

