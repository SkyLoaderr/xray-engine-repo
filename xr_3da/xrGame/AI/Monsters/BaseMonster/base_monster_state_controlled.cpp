#include "stdafx.h"
#include "base_monster.h"
#include "base_monster_state.h"
#include "../controlled_entity.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"

#include "../ai_monster_utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterControlled class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
CBaseMonsterControlled::CBaseMonsterControlled(CBaseMonster *p)
{
	pMonster = p;
	SetPriority(PRIORITY_HIGH);
}

void CBaseMonsterControlled::Init()
{
	inherited::Init ();

	enemy				= 0;
	m_tAction			= m_tPrevAction = ACTION_RUN;
	LastTimeRebuild		= 0;
	FaceEnemyLastTime	= 0;

	pMonster->CMonsterMovement::initialize_movement();

}

void CBaseMonsterControlled::Run()
{
	CControlledEntityBase *entity = smart_cast<CControlledEntityBase *>(pMonster);
	VERIFY(entity);

	switch (entity->get_data().m_task) {
		case eTaskFollow:
			
			enemy = entity->get_data().m_object;
			ExecuteFollow();

			break;
		case eTaskAttack:
			
			enemy = entity->get_data().m_object;
			ExecuteAttack();
			
			break;
		default:

			pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	} 
}

void CBaseMonsterControlled::ExecuteAttack()
{
	// -----------------------------------------------------------------------
	// обновить минимальную и максимальную дистанции до врага
	float m_fDistMin, m_fDistMax;
	float dist = pMonster->GetEnemyDistances(m_fDistMin, m_fDistMax, enemy);

	// определить переменные машины состояний

	if ((m_tPrevAction == ACTION_MELEE) && (dist < m_fDistMax)) m_tAction = ACTION_MELEE;
	else m_tAction = ((dist > m_fDistMin) ? ACTION_RUN : ACTION_MELEE);
	
	bool	squad_active = false;
	CMonsterSquad	*pSquad = 0;

	switch (m_tAction)	{
		// ************
		case ACTION_RUN: 		 // бежать на врага
		// ************	
			pMonster->set_action							(ACT_RUN);
			pMonster->MotionMan.accel_activate				(eAT_Aggressive);
			pMonster->MotionMan.accel_set_braking			(false);

			pMonster->CMonsterMovement::set_target_point	(pMonster->EnemyMan.get_enemy_position(), pMonster->EnemyMan.get_enemy_vertex());
			pMonster->CMonsterMovement::set_rebuild_time	(100 + u32(50.f * dist));
			pMonster->CMonsterMovement::set_distance_to_end	(2.5f);
			pMonster->CMonsterMovement::set_use_covers		();
			pMonster->CMonsterMovement::set_cover_params	(5.f, 30.f, 1.f, 30.f);
			pMonster->CMonsterMovement::set_try_min_time	(false);

			pSquad = monster_squad().get_squad(pMonster);
			squad_active = pSquad && pSquad->SquadActive();

			// Получить команду
			SSquadCommand command;
			pSquad->GetCommand(pMonster, command);
			if (!squad_active || (command.type != SC_ATTACK)) squad_active = false;


			if (squad_active) {
				pMonster->set_use_dest_orient	(true);
				pMonster->set_dest_direction	(command.direction);
			}

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);

			break;
			
		// *********************
		case ACTION_MELEE:		// атаковать вплотную
		// *********************
			
			pMonster->MotionMan.m_tAction	= ACT_ATTACK;

			// Смотреть на врага 
			DO_IN_TIME_INTERVAL_BEGIN(FaceEnemyLastTime, 1200);
				pMonster->FaceTarget(enemy);
			DO_IN_TIME_INTERVAL_END();

			pMonster->CSoundPlayer::play(MonsterSpace::eMonsterSoundAttack, 0,0,pMonster->get_sd()->m_dwAttackSndDelay);
			break;
	}
	
	m_tPrevAction = m_tAction;
}

void CBaseMonsterControlled::ExecuteFollow()
{
	float dist = pMonster->Position().distance_to(enemy->Position());	
	
	if ((pMonster->MotionMan.m_tAction == ACT_STAND_IDLE) && (dist < 15.f)) 
		pMonster->MotionMan.m_tAction = ACT_STAND_IDLE;
	else 
		pMonster->MotionMan.m_tAction = ((dist > 10.0f) ? ACT_WALK_FWD : ACT_STAND_IDLE);

	if (pMonster->MotionMan.m_tAction == ACT_WALK_FWD){
		pMonster->MoveToTarget(random_position(enemy->Position(), 10.f));
	}

}

