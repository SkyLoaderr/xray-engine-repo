#pragma once

#include "../states/state_move_to_point.h"
#include "../states/state_look_point.h"
#include "../states/state_custom_action.h"

#include "chimera_state_threaten_steal.h"
#include "chimera_state_threaten_walk.h"
#include "chimera_state_threaten_roar.h"


#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimeraThreatenAbstract CStateChimeraThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::CStateChimeraThreaten(_Object *obj) : inherited(obj)
{
	add_state(eStateWalk,		xr_new<CStateChimeraThreatenWalk<_Object> >	(obj));
	add_state(eStateThreaten,	xr_new<CStateChimeraThreatenRoar<_Object> >	(obj));
	add_state(eStateSteal,		xr_new<CStateChimeraThreatenSteal<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::~CStateChimeraThreaten()
{
}

#define MIN_DIST_TO_ENEMY	3.f
#define MAX_DIST_TO_ENEMY	15.f
#define MORALE_THRESHOLD	0.8f

TEMPLATE_SPECIALIZATION
bool CStateChimeraThreatenAbstract::check_start_conditions()
{
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) > MAX_DIST_TO_ENEMY) return false;
	if (object->Position().distance_to(object->EnemyMan.get_enemy_position()) < MIN_DIST_TO_ENEMY) return false;
	if (object->GetEntityMorale() < MORALE_THRESHOLD)	return false;
	if (object->HitMemory.is_hit())						return false;
	if (object->hear_dangerous_sound)					return false;
	
	return true;
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::initialize()
{
	inherited::initialize	();
	object->SetUpperState	();
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::reselect_state()
{
	if (prev_substate == u32(-1)) {
		select_state(eStateThreaten);
		return;
	}

	if (prev_substate == eStateSteal) {
		select_state(eStateThreaten);
		return;
	}

	if (prev_substate == eStateThreaten) {
		if (get_state(eStateSteal)->check_start_conditions()) {
			select_state(eStateSteal);
			return;
		} else if (get_state(eStateWalk)->check_start_conditions()) {
			select_state(eStateWalk);
			return;
		}
	}

	select_state(eStateThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::finalize()
{
	inherited::finalize		();
	object->SetUpperState	(false);
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->SetUpperState	(false);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimeraThreatenAbstract
