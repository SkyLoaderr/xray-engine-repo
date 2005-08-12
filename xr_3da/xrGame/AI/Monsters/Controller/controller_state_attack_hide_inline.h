#pragma once

#include "../../../ai_space.h"
#include "../../../cover_point.h"
#include "../../../cover_manager.h"
#include "../../../level.h"
#include "../../../level_debug.h"




#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>
#define CStateControllerHideAbstract CStateControlHide<_Object>

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::initialize()
{
	inherited::initialize();
	
	m_cover_reached		= false;
	select_target_point();
	object->path().prepare_builder();
	
}

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::execute()
{
	object->path().set_target_point		(target.position, target.node);
	object->path().set_rebuild_time		(0);
	object->path().set_distance_to_end	(0.f);
	object->path().set_use_covers		(false);

	object->anim().accel_activate		(eAT_Aggressive);
	object->anim().accel_set_braking	(false);
	
	object->sound().play				(MonsterSpace::eMonsterSoundAttack, 0,0,object->db().m_dwAttackSndDelay);
	object->custom_dir().head_look_point(object->EnemyMan.get_enemy_position());

	object->custom_anim().set_body_state(CControllerAnimation::eTorsoRun,CControllerAnimation::eLegsTypeRun);
}

TEMPLATE_SPECIALIZATION
bool CStateControllerHideAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::reinit()
{
	inherited::reinit();
	m_time_finished = 0;
}


TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::finalize()
{
	inherited::finalize();
	m_time_finished	= Device.dwTimeGlobal;
}


TEMPLATE_SPECIALIZATION
bool CStateControllerHideAbstract::check_completion()
{
	return ((object->ai_location().level_vertex_id() == target.node) && !object->control().path_builder().is_moving_on_path());
}

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::select_target_point()
{
#ifdef DEBUG
	DBG().level_info(this).clear();
#endif

	object->m_ce_best->setup	(object->EnemyMan.get_enemy_position(),10.f,30.f);
	CCoverPoint					*point = ai().cover_manager().best_cover(object->Position(),30.f,*object->m_ce_best);
	//VERIFY(point);
	if (point) {
		target.node					= point->level_vertex_id	();
		target.position				= point->position			();
	} else {
		target.node					= 0;
		target.position				= ai().level_graph().vertex_position(target.node);			
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerHideAbstract
