#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackRunAbstract CStateMonsterAttackRun<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackRunAbstract::CStateMonsterAttackRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackRunAbstract::~CStateMonsterAttackRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::initialize()
{
	inherited::initialize();
	object->CMonsterMovement::initialize_movement	();	
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAbstract::execute()
{
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());
	
	// установка параметров функциональных блоков
	object->set_action									(ACT_RUN);
	object->MotionMan.accel_activate					(eAT_Aggressive);
	object->MotionMan.accel_set_braking					(false);
	object->CMonsterMovement::set_target_point			(object->EnemyMan.get_enemy_position(), object->EnemyMan.get_enemy_vertex());
	object->CMonsterMovement::set_rebuild_time			(100 + u32(50.f * dist));
	object->CMonsterMovement::set_distance_to_end		(2.5f);
	object->CMonsterMovement::set_use_covers			();
	object->CMonsterMovement::set_cover_params			(5.f, 30.f, 1.f, 30.f);
	object->CMonsterMovement::set_try_min_time			(false);
	object->set_state_sound								(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack :: Run", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAbstract::check_completion()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	if (dist < m_fDistMin)	return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAbstract::check_start_conditions()
{
	float m_fDistMin, m_fDistMax, dist;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);
	
	if (dist > m_fDistMax)	return true;

	return false;
}

