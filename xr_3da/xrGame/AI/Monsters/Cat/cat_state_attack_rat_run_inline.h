#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateCatAttackRatRunAbstract CStateCatAttackRatRun<_Object>

TEMPLATE_SPECIALIZATION
CStateCatAttackRatRunAbstract::CStateCatAttackRatRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
void CStateCatAttackRatRunAbstract::initialize()
{
	inherited::initialize();
	object->CMonsterMovement::initialize_movement	();	
}

TEMPLATE_SPECIALIZATION
void CStateCatAttackRatRunAbstract::execute()
{
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());
	
	// установка параметров функциональных блоков
	object->set_action									(ACT_RUN);
	object->MotionMan.accel_deactivate					();
	object->CMonsterMovement::set_target_point			(object->EnemyMan.get_enemy()->Position(), object->EnemyMan.get_enemy()->ai_location().level_vertex_id());
	object->CMonsterMovement::set_rebuild_time			(100 + u32(50.f * dist));
	object->CMonsterMovement::set_distance_to_end		(1.5f);
	object->CMonsterMovement::set_use_covers			();
	object->CMonsterMovement::set_cover_params			(5.f, 30.f, 1.f, 30.f);
	object->CMonsterMovement::set_try_min_time			(false);
	object->set_state_sound								(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Attack Rat:: Run", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}
