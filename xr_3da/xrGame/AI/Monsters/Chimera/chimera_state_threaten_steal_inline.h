#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateChimeraThreatenStealAbstract CStateChimeraThreatenSteal<_Object>

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenStealAbstract::initialize()
{
	inherited::initialize();
	
	object->SetUpperState	(false);

	data.action.action		= ACT_STEAL;
	
	data.accelerated		= true;
	data.braking			= false;
	data.accel_type 		= eAT_Calm;
	
	data.completion_dist	= 2.f;
	data.action.sound_type	= MonsterSpace::eMonsterSoundIdle;
	data.action.sound_delay = object->get_sd()->m_dwIdleSndDelay;
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenStealAbstract::finalize()
{
	inherited::finalize();
	object->SetUpperState	();
}


TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenStealAbstract::execute()
{
	data.point				= object->EnemyMan.get_enemy_position	();
	data.vertex				= object->EnemyMan.get_enemy_vertex		();
	data.time_to_rebuild	= object->get_attack_rebuild_time		();

	inherited::execute();

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Threaten :: Steal", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif

}

#define MIN_DISTANCE_TO_ENEMY	8.f

TEMPLATE_SPECIALIZATION
bool CStateChimeraThreatenStealAbstract::check_completion()
{	
	if (inherited::check_completion()) return true;
	
	float dist_to_enemy = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist_to_enemy < MIN_DISTANCE_TO_ENEMY) return true;

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateChimeraThreatenStealAbstract::check_start_conditions()
{
	float dist_to_enemy = object->EnemyMan.get_enemy_position().distance_to(object->Position());
	if (dist_to_enemy > MIN_DISTANCE_TO_ENEMY) return true;
	return false;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateChimeraThreatenStealAbstract

