#pragma once

#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackRunAbstract CStateMonsterAttackRun<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::initialize()
{
	inherited::initialize();
	object->movement().initialize_movement	();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::execute()
{
	// установка параметров функциональных блоков
	object->set_action							(ACT_RUN);
	object->MotionMan.accel_activate			(eAT_Aggressive);
	object->MotionMan.accel_set_braking			(false);
	object->movement().set_target_point			(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	object->movement().set_rebuild_time			(object->get_attack_rebuild_time());
	object->movement().set_use_covers			();
	object->movement().set_cover_params			(0.1f, 30.f, 1.f, 30.f);
	object->movement().set_try_min_time			(false);
	object->set_state_sound						(MonsterSpace::eMonsterSoundAttack);
	object->movement().extrapolate_path			(true);

	
	// обработать squad инфо	
	CMonsterSquad *squad	= monster_squad().get_squad(object);
	if (squad && squad->SquadActive()) {
		// Получить команду
		SSquadCommand command;
		squad->GetCommand(object, command);
		
		if (command.type == SC_ATTACK) {
			object->movement().set_use_dest_orient	(true);
			object->movement().set_dest_direction	(command.direction);
		}
	}

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Attack :: Run", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::finalize()
{
	inherited::finalize					();
	object->movement().extrapolate_path	(false);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::critical_finalize()
{
	inherited::critical_finalize		();
	object->movement().extrapolate_path	(false);
}


TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAbstract::check_completion()
{
	float m_fDistMin	= object->MeleeChecker.get_min_distance		();
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());

	if (dist < m_fDistMin)	return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAbstract::check_start_conditions()
{
	float m_fDistMax	= object->MeleeChecker.get_max_distance		();
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());

	if (dist > m_fDistMax)	return true;

	return false;
}

