#include "stdafx.h"
#include "chimera.h"
#include "chimera_state_manager.h"
#include "../ai_monster_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"

#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"

#include "chimera_state_threaten.h"

CStateManagerChimera::CStateManagerChimera(CChimera *obj) : inherited(obj)
{
	//add_state(
	//	eStateRest, 
	//	xr_new<CStateMonsterRest<CChimera> > (obj, 
	//		xr_new<CStateMonsterRestSleep<CChimera> >(obj), 
	//		xr_new<CStateMonsterRestWalkGraph<CChimera> >(obj)
	//	)
	//);

	//add_state(
	//	eStateAttack, xr_new<CStateMonsterAttack<CChimera> > (obj,
	//		xr_new<CStateMonsterAttackRun<CChimera> >(obj), 
	//		xr_new<CStateMonsterAttackMelee<CChimera> >(obj)
	//	)
	//);

	//add_state(
	//	eStateFindEnemy, xr_new<CStateMonsterFindEnemy<CChimera> > (obj,
	//		xr_new<CStateMonsterFindEnemyRun<CChimera> >(obj), 
	//		xr_new<CStateMonsterFindEnemyLook<CChimera> >(obj,
	//			xr_new<CStateMonsterMoveToPoint<CChimera> >(obj), 
	//			xr_new<CStateMonsterCustomAction<CChimera> >(obj),
	//			xr_new<CStateMonsterLookToPoint<CChimera> >(obj)),
	//		xr_new<CStateMonsterFindEnemyAngry<CChimera> >(obj), 
	//		xr_new<CStateMonsterFindEnemyWalkAround<CChimera> >(obj)
	//	)
	//);
	

	add_state(eStateRest,				xr_new<CStateMonsterRest<CChimera> >					(obj));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CChimera> >					(obj));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CChimera> >					(obj));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CChimera> >						(obj));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CChimera> >	(obj));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CChimera> >		(obj));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CChimera> >					(obj));


//	add_state(
//		eStateThreaten, xr_new<CStateChimeraThreaten<CChimera> > (
//			xr_new<CStateMonsterThreatenWalk<CChimera> >, 
//			xr_new<CStateMonsterThreatenThreaten<CChimera> >
//		)	
//	);
//
//	add_state(
//		eStatePanic, xr_new<CStateMonsterPanic<CChimera> > (
//			xr_new<CStateMonsterPanicRun<CChimera> > 
//		)
//	);
}

CStateManagerChimera::~CStateManagerChimera()
{
}

void CStateManagerChimera::execute()
{
	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	const CEntityAlive* corpse	= object->CorpseMan.get_corpse();


	if (enemy) {
		switch (object->EnemyMan.get_danger_type()) {
			case eVeryStrong:	state_id = eStatePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:			state_id = eStatePanic; break;
		}

	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateDangerousSound;
	} else if (object->hear_interesting_sound) {
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
		else state_id = eStateRest;
	}


	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

