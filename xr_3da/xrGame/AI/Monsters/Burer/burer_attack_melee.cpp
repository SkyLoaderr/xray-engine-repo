#include "stdafx.h"
#include "../ai_monster_state.h"
#include "burer_attack_melee.h"
#include "burer.h"

#define MIN_DIST_MELEE_ATTACK	5.f
#define MAX_DIST_MELEE_ATTACK	9.f

CBurerAttackMelee::CBurerAttackMelee(CBurer *p)
{
	pMonster = p;
}

void CBurerAttackMelee::Init()
{
	inherited::Init();

	enemy = pMonster->EnemyMan.get_enemy();	

	time_enemy_last_faced	= 0;
	time_path_last_rebuild	= 0;

	m_tAction = ACTION_RUN;
}

//////////////////////////////////////////////////////////////////////////
// √лавный цикл выполнени€
//////////////////////////////////////////////////////////////////////////

void CBurerAttackMelee::Run()
{

	// ≈сли ничего не выбрали
	float m_fDistMin, m_fDistMax;
	float dist;
	dist = pMonster->GetEnemyDistances(m_fDistMin, m_fDistMax);

	// получить минимальную и максимальную дистанции до врага
	if ((m_tAction == ACTION_MELEE) && (dist < m_fDistMax)) m_tAction = ACTION_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_MELEE);
	
	switch (m_tAction) {
		// *********************
		case ACTION_MELEE:		
		// *********************
			
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;

			// —мотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(time_enemy_last_faced, 1200);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

			break;

		// ************
		case ACTION_RUN:		 // бежать на врага
		// ************	
			pMonster->MotionMan.m_tAction	= ACT_RUN;
			pMonster->CMonsterMovement::set_try_min_time(false);

			pMonster->MotionMan.accel_activate		(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking	(false);

			pMonster->CMonsterMovement::set_target_point	(pMonster->EnemyMan.get_enemy_position(), pMonster->EnemyMan.get_enemy_vertex());
			pMonster->CMonsterMovement::set_rebuild_time	(100 + u32(50.f * dist));
			pMonster->CMonsterMovement::set_distance_to_end	(2.5f);
			pMonster->CMonsterMovement::set_use_covers		();
			pMonster->CMonsterMovement::set_cover_params	(5.f, 30.f, 1.f, 30.f);

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

			break;
	}
}

void CBurerAttackMelee::Done()
{
}

void CBurerAttackMelee::UpdateExternal()
{
	enemy = pMonster->EnemyMan.get_enemy();	
}

bool CBurerAttackMelee::IsCompleted()
{
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist < MAX_DIST_MELEE_ATTACK) return false;

	return true;
}

bool CBurerAttackMelee::CheckStartCondition () 
{
	float dist = pMonster->Position().distance_to(enemy->Position());
	if (dist > MIN_DIST_MELEE_ATTACK) return false;

	return true;
}
