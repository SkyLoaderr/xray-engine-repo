#include "stdafx.h"
#include "../../ai_monster_state.h"
#include "burer_attack_gravi.h"
#include "burer.h"

#define GOOD_DISTANCE_FOR_GRAVI 8.f
#define GRAVI_DELAY				5000

CBurerAttackGravi::CBurerAttackGravi(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackGravi::Init()
{
	inherited::Init();
	UpdateExternal();
	
	m_tAction					= ACTION_GRAVI_STARTED;

	time_gravi_started			= 0;
	time_enemy_last_faced		= 0;

}

void CBurerAttackGravi::Run()
{

	switch (m_tAction) {
		/************************/
		case ACTION_GRAVI_STARTED:
		/************************/

			ExecuteGraviStart();
			m_tAction = ACTION_GRAVI_CONTINUE;

			break;
		/************************/
		case ACTION_GRAVI_CONTINUE:
		/************************/

			ExecuteGraviContinue();

			break;

		/************************/
		case ACTION_GRAVI_FIRE:
		/************************/

			ExecuteGraviFire();
			m_tAction = ACTION_WAIT_TRIPLE_END;

			break;
		/***************************/
		case ACTION_WAIT_TRIPLE_END:
		/***************************/

			if (!pMonster->MotionMan.TA_IsActive()) {
				m_tAction = ACTION_COMPLETED; 
			}

		/*********************/
		case ACTION_COMPLETED:
		/*********************/

			break;
	}

	pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;	

	DO_IN_TIME_INTERVAL_BEGIN(time_enemy_last_faced, 1200);
		pMonster->FaceTarget(enemy);
	DO_IN_TIME_INTERVAL_END();

}

void CBurerAttackGravi::Done()
{

}


bool CBurerAttackGravi::CheckStartCondition()
{
	// ���������� �������
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist < GOOD_DISTANCE_FOR_GRAVI) return false;

	bool see_enemy_now		= pMonster->EnemyMan.get_enemy_time_last_seen() == Level().timeServer();

	if (!see_enemy_now) return false; 

	// �� ��, ����� ������ ����� �����
	return true;
}


bool CBurerAttackGravi::IsCompleted()
{
	return (m_tAction == ACTION_COMPLETED);
}

void CBurerAttackGravi::CriticalInterrupt()
{
	pMonster->MotionMan.TA_Deactivate();
}

void CBurerAttackGravi::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}


// ��������� ���������
void CBurerAttackGravi::ExecuteGraviStart()
{
	pMonster->MotionMan.TA_Activate(&pMonster->anim_triple_gravi);
	
	time_gravi_started			= m_dwCurrentTime;
	
	pMonster->StartGraviPrepare();
}

void CBurerAttackGravi::ExecuteGraviContinue()
{
	// ��������� �� ����� ����
	if (time_gravi_started + pMonster->m_gravi_time_to_hold < m_dwCurrentTime) {
		m_tAction = ACTION_GRAVI_FIRE;
	}
}

void CBurerAttackGravi::ExecuteGraviFire()
{
	pMonster->MotionMan.TA_PointBreak();
	
	Fvector from_pos;
	Fvector target_pos;
	from_pos	= pMonster->Position();	from_pos.y		+= 0.5f;
	target_pos	= enemy->Position();	target_pos.y	+= 0.5f;
	
	pMonster->m_gravi_object.activate(enemy, from_pos, target_pos);
	
	pMonster->StopGraviPrepare();
	pMonster->CSoundPlayer::play(eMonsterSoundGraviAttack);
}
