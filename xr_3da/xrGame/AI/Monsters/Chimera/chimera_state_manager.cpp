#include "stdafx.h"
#include "chimera.h"
#include "chimera_state_manager.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"

CStateManagerChimera::CStateManagerChimera(CChimera *obj) : inherited(obj)
{
	add_state(
		eStateRest, 
		xr_new<CStateMonsterRest<CChimera> > (obj, 
			xr_new<CStateMonsterRestSleep<CChimera> >(obj), 
			xr_new<CStateMonsterRestWalkGraph<CChimera> >(obj)
		)
	);

//	add_state(
//		eStateAttack, xr_new<CStateMonsterAttack<CChimera> > (
//			xr_new<CStateMonsterAttackRun<CChimera> >, 
//			xr_new<CStateMonsterAttackMelee<CChimera> >
//		)
//	);
//
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
	u32 state = u32(-1);

	if (object->EnemyMan.get_enemy()) {
		switch (object->EnemyMan.get_danger_type()) {
			case eVeryStrong:				state = eStatePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:						state = eStateAttack; break;
		}
	} else if (object->hear_dangerous_sound || object->hear_interesting_sound) {
		if (object->hear_dangerous_sound)			state = eStateInterestingSound;		
		if (object->hear_interesting_sound)			state = eStateInterestingSound;	
	} else	if (object->CorpseMan.get_corpse() && ((object->GetSatiety() < object->_sd->m_fMinSatiety) || object->flagEatNow))					
		state = eStateEat;	
	else									state = eStateRest;

	state = eStateRest;

	select_state(state); 
}

