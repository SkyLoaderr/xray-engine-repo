#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireApproachAbstract CStateBloodsuckerVampireApproach<_Object>

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireApproachAbstract::CStateBloodsuckerVampireApproach(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireApproachAbstract::~CStateBloodsuckerVampireApproach()
{
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireApproachAbstract::initialize()
{
	inherited::initialize();
	object->movement().initialize_movement	();	
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireApproachAbstract::execute()
{
	// установка параметров функциональных блоков
	object->set_action									(ACT_RUN);
	object->MotionMan.accel_activate					(eAT_Aggressive);
	object->MotionMan.accel_set_braking					(false);
	object->movement().set_target_point					(object->EnemyMan.get_enemy()->Position(), object->EnemyMan.get_enemy()->ai_location().level_vertex_id());
	object->movement().set_rebuild_time					(object->get_attack_rebuild_time());
	object->movement().set_use_covers					(false);
	object->movement().set_distance_to_end				(2.f);
	object->set_state_sound								(MonsterSpace::eMonsterSoundAttack);

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		object->HDebug->M_Add(0,"Vampire :: Approach", D3DCOLOR_XRGB(255,0,0));
	}
#endif

}

