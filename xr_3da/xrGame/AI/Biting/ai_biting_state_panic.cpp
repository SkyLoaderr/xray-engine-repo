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

	VERIFY(pMonster->EnemyMan.get_enemy());
	position = pMonster->EnemyMan.get_enemy_position();
		
	m_tAction		= ACTION_RUN;

	target_vertex_id			= u32(-1);
	last_time_cover_selected	= 0;
}

void CBitingPanic::Run()
{
	VERIFY(pMonster->EnemyMan.get_enemy());
	
	if (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime) {
		position = pMonster->EnemyMan.get_enemy_position();
		target_vertex_id = u32(-1);
	} 

	if (target_vertex_id == u32(-1) && (last_time_cover_selected + 5000 < m_dwCurrentTime)) {
		CTimer T; T.Start();
		if (!pMonster->GetCoverFromEnemy(position, target_pos, target_vertex_id)) {
			target_vertex_id			= u32(-1);
			last_time_cover_selected	= m_dwCurrentTime;
		}
		//Msg("GetCoverFromEnemy = [%fs]", T.GetElapsed_sec());
	}

	switch (m_tAction) {
		
		/**************/
		case ACTION_RUN:
		/**************/
		
			LOG_EX("PANIC:: Run away");
			pMonster->MotionMan.m_tAction	= ACT_RUN;

			if (target_vertex_id != u32(-1)) {
				pMonster->MoveToTarget			(target_pos, target_vertex_id);
			} else {
				pMonster->MoveAwayFromTarget	(position);
			}
			
			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);

			//if (!pMonster->MotionStats->is_good_motion(3)) m_tAction = ACTION_FACE_BACK_SCARED;
			if (pMonster->EnemyMan.get_enemy_time_last_seen() + 10000 < m_dwCurrentTime) m_tAction = ACTION_FACE_BACK_SCARED;
			if (pMonster->IsPathEnd(2.5f)) target_vertex_id = u32(-1);
			
			break;
		/***************************/
		case ACTION_FACE_BACK_SCARED:
		/***************************/

			LOG_EX("PANIC:: Face Back scared!");
			pMonster->MotionMan.SetSpecParams(ASP_STAND_SCARED);
			pMonster->MotionMan.m_tAction	= ACT_STAND_IDLE;

			pMonster->FaceTarget(position);

			if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(10)) {
				if (pMonster->EnemyMan.get_enemy_time_last_seen() == m_dwCurrentTime) m_tAction = ACTION_RUN;
			}

			break;
	}
	
	pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundPanic, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
}

bool CBitingPanic::CheckCompletion()
{
	return false;
}

