#pragma once

#include "../../../sound_player.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateZombieAttackRunAbstract CStateZombieAttackRun<_Object>

TEMPLATE_SPECIALIZATION
CStateZombieAttackRunAbstract::CStateZombieAttackRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateZombieAttackRunAbstract::~CStateZombieAttackRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateZombieAttackRunAbstract::initialize()
{
	inherited::initialize();
	m_time_path_rebuild	= 0;

	if (object->HitMemory.is_hit() && (object->conditions().GetHealth() < 0.5f)) {
		action = ACT_RUN;
	} else {
		action = ACT_WALK_FWD;
	}
	object->movement().initialize_movement();
}

TEMPLATE_SPECIALIZATION
void CStateZombieAttackRunAbstract::execute()
{
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());
	
	object->movement().set_try_min_time	(false);
	
	// ��������� ���������� �������������� ������
	object->movement().set_target_point			(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	object->movement().set_rebuild_time			(100 + u32(50.f * dist));
	object->movement().set_distance_to_end		(2.5f);
	object->movement().set_use_covers			(false);

	//////////////////////////////////////////////////////////////////////////
	// ���������� squad-������
	//////////////////////////////////////////////////////////////////////////
	CMonsterSquad *squad	= monster_squad().get_squad(object);
	bool squad_active		= squad && squad->SquadActive();
	
	// �������� �������
	SSquadCommand command;
	squad->GetCommand(object, command);
	if (!squad_active || (command.type != SC_ATTACK)) squad_active = false;
	//////////////////////////////////////////////////////////////////////////

	if (squad_active) {
		object->movement().set_use_dest_orient	(true);
		object->movement().set_dest_direction	(command.direction);
	}
	
	// ��������� ���������� �������������� ������
	object->MotionMan.m_tAction					= action;	
	if (action == ACT_RUN) 
		object->movement().set_try_min_time	(true);
	
	object->sound().play						(MonsterSpace::eMonsterSoundAttack, 0,0,object->get_sd()->m_dwAttackSndDelay);
	object->MotionMan.accel_activate			(eAT_Aggressive);
	object->MotionMan.accel_set_braking			(false);
}

TEMPLATE_SPECIALIZATION
bool CStateZombieAttackRunAbstract::check_completion()
{
	float m_fDistMin	= object->MeleeChecker.get_min_distance		();
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());

	if (dist < m_fDistMin)	return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateZombieAttackRunAbstract::check_start_conditions()
{
	float m_fDistMax	= object->MeleeChecker.get_max_distance		();
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());
	
	if (dist > m_fDistMax)	return true;

	return false;
}

