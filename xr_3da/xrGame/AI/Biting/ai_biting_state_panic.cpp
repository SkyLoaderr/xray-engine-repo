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

	if (pMonster->EnemyMan.get_enemy())
		position = pMonster->EnemyMan.get_enemy_position();
	else if (pMonster->SoundMemory.IsRememberSound()) 
		position = pMonster->SoundMemory.GetSound().position;
	else 
		position = pMonster->Position();
		
	m_tAction		= ACTION_RUN;
}

void CBitingPanic::Run()
{
	if (pMonster->EnemyMan.get_enemy() && (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime)) {
		position = pMonster->EnemyMan.get_enemy_position();
	}

	float dist = pMonster->Position().distance_to(position);
	
	switch (m_tAction) {
		
		/**************/
		case ACTION_RUN:
		/**************/
		
			LOG_EX("PANIC:: Run away");
			pMonster->MotionMan.m_tAction	= ACT_RUN;

			pMonster->MoveAwayFromTarget			(position);
			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);

			
			if (dist > 50.f) m_tAction = ACTION_FACE_BACK_SCARED;

			break;
		
		/***************************/
		case ACTION_FACE_BACK_SCARED:
		/***************************/

			LOG_EX("PANIC:: Face Back scared!");
			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			pMonster->FaceTarget(position);

			// если враг виден
			if (pMonster->EnemyMan.get_enemy() && (pMonster->EnemyMan.get_enemy_time_last_seen() + 1000 > m_dwCurrentTime)) m_tAction = ACTION_RUN;
			break;
	}
	
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,pMonster->_sd->m_dwAttackSndDelay);
}

bool CBitingPanic::CheckCompletion()
{
	return false;
}

