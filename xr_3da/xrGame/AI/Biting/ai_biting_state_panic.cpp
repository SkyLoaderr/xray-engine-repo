#include "stdafx.h"
#include "ai_biting.h"
#include "ai_biting_state.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingPanic class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBitingPanic::CBitingPanic(CAI_Biting *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBitingPanic::Init()
{
	LOG_EX("PANIC:: Init");
	inherited::Init();

	m_tAction		= ACTION_RUN;
}

void CBitingPanic::Run()
{
	switch (m_tAction) {
		
		/**************/
		case ACTION_RUN:
		/**************/
		
			LOG_EX("PANIC:: Run away");
			pMonster->MotionMan.m_tAction	= ACT_RUN;

			pMonster->MoveAwayFromTarget			(pMonster->EnemyMan.get_enemy_position());
			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);

			// если не видел врага > 15 сек 
			if (pMonster->EnemyMan.get_enemy_time_last_seen() + 10000 < m_dwCurrentTime) {
				m_tAction = ACTION_FACE_BACK_SCARED;
				pMonster->CMovementManager::m_body.target.yaw = angle_normalize(pMonster->CMovementManager::m_body.target.yaw + PI);
			}

			break;
		
		/***************************/
		case ACTION_FACE_BACK_SCARED:
		/***************************/

			LOG_EX("PANIC:: Face Back scared!");
			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			// если враг виден
			if (pMonster->EnemyMan.get_enemy_time_last_seen() + 1000 > m_dwCurrentTime) m_tAction = ACTION_RUN;

			break;
	}
	
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,pMonster->_sd->m_dwAttackSndDelay);
}

bool CBitingPanic::CheckCompletion()
{
//	if (!m_tEnemy.obj) return true;
//	if (pMonster->Position().distance_to(m_tEnemy.obj->Position()) > 30.f) return true;

	return false;
}

