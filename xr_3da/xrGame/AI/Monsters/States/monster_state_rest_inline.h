#pragma once

#include "monster_state_rest_sleep.h"
#include "monster_state_rest_walk_graph.h"
#include "monster_state_rest_idle.h"
#include "monster_state_rest_fun.h"
#include "monster_state_squad_rest.h"
#include "monster_state_squad_rest_follow.h"
#include "state_move_to_restrictor.h"
#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestAbstract CStateMonsterRest<_Object>

#define TIME_DELAY_FUN	20000

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj) : inherited(obj)
{
	add_state(eStateRest_Sleep,			xr_new<CStateMonsterRestSleep<_Object> >	(obj));
	add_state(eStateRest_WalkGraphPoint,xr_new<CStateMonsterRestWalkGraph<_Object> >(obj));
	add_state(eStateRest_Idle,			xr_new<CStateMonsterRestIdle<_Object> >		(obj));
	add_state(eStateRest_Fun,			xr_new<CStateMonsterRestFun<_Object> >		(obj));
	add_state(eStateSquad_Rest,			xr_new<CStateMonsterSquadRest<_Object> >	(obj));
	add_state(eStateSquad_RestFollow,	xr_new<CStateMonsterSquadRestFollow<_Object> >(obj));
	add_state(eStateCustomMoveToRestrictor, xr_new<CStateMonsterMoveToRestrictor<_Object> > (obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj, state_ptr state_sleep, state_ptr state_walk) : inherited(obj)
{
	add_state(eStateRest_Sleep,			state_sleep);
	add_state(eStateRest_WalkGraphPoint,state_walk);
	add_state(eStateRest_Idle,			xr_new<CStateMonsterRestIdle<_Object> >		(obj));
	add_state(eStateRest_Fun,			xr_new<CStateMonsterRestFun<_Object> >		(obj));
	add_state(eStateSquad_Rest,			xr_new<CStateMonsterSquadRest<_Object> >	(obj));
	add_state(eStateSquad_RestFollow,	xr_new<CStateMonsterSquadRestFollow<_Object> >(obj));
	add_state(eStateCustomMoveToRestrictor, xr_new<CStateMonsterMoveToRestrictor<_Object> > (obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::~CStateMonsterRest	()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::initialize()
{
	inherited::initialize	();

	time_last_fun			= 0;
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::execute()
{
	bool move_to_restrictor = false;
	
	if (prev_substate == eStateCustomMoveToRestrictor) {
		if (!get_state(eStateCustomMoveToRestrictor)->check_completion()) 
			move_to_restrictor = true;
	} else if (get_state(eStateCustomMoveToRestrictor)->check_start_conditions()) 
			move_to_restrictor = true;
	
	if (move_to_restrictor) select_state(eStateCustomMoveToRestrictor);
	else {
		// check squad behaviour
		bool use_squad = false;

		if (monster_squad().get_squad(object)->GetCommand(object).type == SC_REST) {
			select_state	(eStateSquad_Rest);
			use_squad		= true;
		} else if (monster_squad().get_squad(object)->GetCommand(object).type == SC_FOLLOW) {
			select_state	(eStateSquad_RestFollow);
			use_squad		= true;
		} 

		if (!use_squad) {
			bool bNormalSatiety =	(object->conditions().GetSatiety() > object->db().m_fMinSatiety) && 
				(object->conditions().GetSatiety() < object->db().m_fMaxSatiety); 

			bool state_fun = false;

			if (prev_substate == eStateRest_Fun) {
				if (!get_state(eStateRest_Fun)->check_completion()) 
					state_fun = true;
			} else {
				if (get_state(eStateRest_Fun)->check_start_conditions() && (time_last_fun + TIME_DELAY_FUN < Device.dwTimeGlobal)) 
					state_fun = true;
			}

			if (state_fun) {
				select_state	(eStateRest_Fun);
			} else {
				if (bNormalSatiety) {
					select_state	(eStateRest_Idle);
				} else {
					select_state	(eStateRest_WalkGraphPoint);
				}
			}

			if ((prev_substate == eStateRest_Fun) && (current_substate != prev_substate)) time_last_fun = Device.dwTimeGlobal;
		}
	}


	get_state_current()->execute();
	prev_substate = current_substate;
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterRestAbstract