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

	object->anim().m_tAction			= ACT_RUN;
	object->anim().accel_activate		(eAT_Aggressive);
	object->anim().accel_set_braking	(false);
	
	object->sound().play				(MonsterSpace::eMonsterSoundAttack, 0,0,object->db().m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
bool CStateControllerHideAbstract::check_start_conditions()
{
	if (m_time_finished + 1000 > Device.dwTimeGlobal) return false;
	
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
	return (object->ai_location().level_vertex_id() == target.node);
}

TEMPLATE_SPECIALIZATION
void CStateControllerHideAbstract::select_target_point()
{
	DBG().level_info(this).clear();
	
	object->m_ce_best->setup	(Level().CurrentEntity()->Position(),10.f,30.f);
	CCoverPoint					*point = ai().cover_manager().best_cover(object->Position(),30.f,*object->m_ce_best,CControllerCoverPredicate());
	VERIFY(point);

	target.node					= point->level_vertex_id();
	target.position				= point->position();

	Fvector cur_pos = point->position();
	float	r = 0.5f;
	for (u32 i = 0; i< 5; i++) {
		DBG().level_info(this).add_item(cur_pos, r, D3DCOLOR_XRGB(0,0,255));
		DBG().level_info(this).add_item(cur_pos, r+0.05f, D3DCOLOR_XRGB(0,0,255));
		cur_pos.mad(Fvector().set(0.f,1.f,0.f), r * 2);
	}

}


#define CStateTest1Abstract CStateTest1<_Object>

TEMPLATE_SPECIALIZATION
void CStateTest1Abstract::execute()
{
	object->set_action			(ACT_STAND_IDLE);
	object->dir().face_target	(Level().CurrentEntity()->Position(), 1200);
	
}

TEMPLATE_SPECIALIZATION
void CStateTest1Abstract::reinit()
{
	inherited::reinit();
	m_time_state = 0;
}

TEMPLATE_SPECIALIZATION
void CStateTest1Abstract::initialize()
{
	inherited::initialize();
	m_time_state = Device.dwTimeGlobal;
}

TEMPLATE_SPECIALIZATION
bool CStateTest1Abstract::check_completion()
{
	return (m_time_state + 2000 < Device.dwTimeGlobal);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerHideAbstract
