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
	object->movement().initialize_movement	();	
}

TEMPLATE_SPECIALIZATION
void CStateCatAttackRatRunAbstract::execute()
{
	float dist = object->EnemyMan.get_enemy()->Position().distance_to(object->Position());
	
	// установка параметров функциональных блоков
	object->set_action									(ACT_RUN);
	object->MotionMan.accel_deactivate					();
	object->movement().set_target_point			(object->EnemyMan.get_enemy()->Position(), object->EnemyMan.get_enemy()->ai_location().level_vertex_id());
	object->movement().set_rebuild_time			(100 + u32(50.f * dist));
	object->movement().set_distance_to_end		(1.5f);
	object->movement().set_use_covers			();
	object->movement().set_cover_params			(5.f, 30.f, 1.f, 30.f);
	object->movement().set_try_min_time			(false);
	object->set_state_sound								(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Attack Rat:: Run", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif

}
