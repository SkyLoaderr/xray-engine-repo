#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterFindEnemyRunAbstract CStateMonsterFindEnemyRun<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyRunAbstract::CStateMonsterFindEnemyRun(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateMonsterFindEnemyRunAbstract::~CStateMonsterFindEnemyRun()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyRunAbstract::initialize()
{
	inherited::initialize					();

	object->movement().initialize_movement	();	
	
	
	target_point	= object->EnemyMan.get_enemy_position();
	target_vertex	= object->EnemyMan.get_enemy_vertex();

	Fvector dir;
	dir.sub(target_point, object->Position());
	dir.normalize();
	
	Fvector test_position;
	test_position.mad(target_point, dir, 10.f);
	
	// провериь возможность пробежать дальше
	if (ai().level_graph().valid_vertex_position(test_position)) {
		u32 vertex_id = ai().level_graph().vertex_id(test_position);
		if (ai().level_graph().valid_vertex_id(vertex_id)) {
			target_point	= test_position;
			target_vertex	= vertex_id;
		}
	}

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Find Enemy :: Run", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif
}

TEMPLATE_SPECIALIZATION
void CStateMonsterFindEnemyRunAbstract::execute()
{
	object->set_action							(ACT_RUN);
	object->MotionMan.accel_activate			(eAT_Aggressive);
	object->MotionMan.accel_set_braking			(false);
	object->movement().set_target_point			(target_point, target_vertex);
	object->movement().set_rebuild_time			(object->get_attack_rebuild_time());
	object->movement().set_use_covers			();
	object->movement().set_cover_params			(5.f, 30.f, 1.f, 30.f);
	object->movement().set_try_min_time			(false);
	object->set_state_sound						(MonsterSpace::eMonsterSoundAttack);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterFindEnemyRunAbstract::check_completion()
{	
	float dist = object->Position().distance_to(target_point);

	if (dist > 1.5f) return false;
	return true;
}

