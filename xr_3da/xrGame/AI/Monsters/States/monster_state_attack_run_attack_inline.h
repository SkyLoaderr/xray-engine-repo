#pragma once

#include "../critical_action_info.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackRunAttackAbstract CStateMonsterAttackRunAttack<_Object>

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAttackAbstract::initialize()
{
	inherited::initialize			();
	object->CriticalActionInfo->set	(CAF_LockPath);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAttackAbstract::execute()
{
	object->set_action				(ACT_RUN);
	object->set_state_sound			(MonsterSpace::eMonsterSoundAttack);
	object->MotionMan.SetSpecParams	(ASP_ATTACK_RUN);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack :: Attack Run", D3DCOLOR_XRGB(255,0,0));
	}
#endif
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAttackAbstract::finalize()
{
	inherited::finalize					();
	object->CriticalActionInfo->clear	(CAF_LockPath);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackRunAttackAbstract::critical_finalize()
{
	inherited::critical_finalize		();
	object->CriticalActionInfo->clear	(CAF_LockPath);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAttackAbstract::check_start_conditions()
{
	float dist			= object->MeleeChecker.distance_to_enemy	(object->EnemyMan.get_enemy());
	
	if (dist > object->MeleeChecker.get_max_distance())	return false;
	if (dist < object->MeleeChecker.get_min_distance())	return false;
	
	// check angle
	if (!object->DirMan.is_face_target(object->EnemyMan.get_enemy(), deg(5))) return false;
	
	// try to build path
	Fvector target_position;
	target_position.mad(object->Position(), object->Direction(), 8.0f);
	
	if (!object->CMonsterMovement::build_special(target_position, u32(-1), object->eVelocityParamsRunAttack)) return false;
	else object->CMonsterMovement::enable_path();

	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackRunAttackAbstract::check_completion()
{
	if (!object->CMonsterMovement::IsMovingOnPath()) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterAttackRunAttackAbstract
