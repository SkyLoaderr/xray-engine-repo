#include "stdafx.h"
#include "zombie.h"
#include "zombie_state_manager.h"
#include "../../ai_monster_squad.h"
#include "../../ai_monster_squad_manager.h"
#include "../../ai_monster_utils.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "zombie_state_attack_run.h"
#include "../states/monster_state_squad_rest.h"
#include "../../ai_monster_debug.h"

CStateManagerZombie::CStateManagerZombie(CZombie *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CZombie> >	(obj));
	add_state(
		eStateAttack,				
		xr_new<CStateMonsterAttack<CZombie> > (obj, 
			xr_new<CStateZombieAttackRun<CZombie> > (obj), 
			xr_new<CStateMonsterAttackMelee<CZombie> > (obj)
		)
	);
	add_state(eStateEat,				xr_new<CStateMonsterEat<CZombie> >(obj));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CZombie> >(obj));
	add_state(eStateSquadRest,			xr_new<CStateMonsterSquadRest<CZombie> >(obj));
}

CStateManagerZombie::~CStateManagerZombie()
{
}

void CStateManagerZombie::initialize()
{
	inherited::initialize();
}

void CStateManagerZombie::execute()
{
	u32 state_id = u32(-1);
	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	CMonsterSquad	*squad = monster_squad().get_squad(object);
	SSquadCommand	command;
	squad->GetCommand(object, command);


	if (enemy) {
		state_id = eStateAttack;
	} else if (object->hear_interesting_sound || object->hear_dangerous_sound) {
		state_id = eStateInterestingSound;
	} else {
		bool can_eat = false;
		if (corpse) {

			if (prev_substate == eStateEat) {
				if (!get_state_current()->check_completion()) can_eat = true;
			}

			if ((prev_substate != eStateEat) && (object->GetSatiety() < object->get_sd()->m_fMinSatiety)) 
				can_eat = true;		
		}

		if (can_eat) state_id = eStateEat;
		else {
			if (squad && squad->SquadActive() && (command.type == SC_REST)) {
				state_id = eStateSquadRest;
			} else 
				state_id = eStateRest;
		}
	}
	
	// установить текущее состояние
	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;

	// информировать squad о своих целях
	squad_notify();
}


void CStateManagerZombie::squad_notify()
{
	CMonsterSquad	*squad = monster_squad().get_squad(object);
	SMemberGoal		goal;
	
	// для определения глобального состояние можно использовать current_substate
	// для определения конкретного типа состояния можно использовать get_state_type()
	
	//VERIFY(get_state_type() != ST_Unknown);

	if (current_substate == eStateAttack) {
	
		goal.type	= MG_AttackEnemy;
		goal.entity	= const_cast<CEntityAlive*>(object->EnemyMan.get_enemy());
	
	} else if (current_substate == eStateRest) {
		
		if (get_state_type() == ST_Rest) {
			
			goal.type	= MG_Rest;

		} else if (get_state_type() == ST_WalkGraphPoint) {

			goal.type	= MG_WalkGraph;

		}
	} else if (current_substate == eStateSquadRest) {
		
		goal.type	= MG_Rest;

	}

	squad->UpdateGoal(object, goal);
}
