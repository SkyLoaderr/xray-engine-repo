#include "stdafx.h"
#include "chimera.h"
#include "../States/monster_state_rest.h"
#include "../States/monster_state_rest_sleep.h"
#include "../States/monster_state_rest_walk_graph.h"
#include "../States/monster_state_attack.h"
#include "../States/monster_state_attack_run.h"
#include "../States/monster_state_attack_melee.h"

#include "chimera_state_threaten.h"
#include "../States/monster_state_threaten_walk.h"
#include "../States/monster_state_threaten_threaten.h"

#include "../States/monster_state_panic.h"
#include "../States/monster_state_panic_run.h"

CStateManagerChimera::CStateManagerChimera	(LPCSTR state_name) : inherited(state_name)
{
}

CStateManagerChimera::~CStateManagerChimera	()
{
}

void CStateManagerChimera::Load				(LPCSTR section)
{
	// set hierarchy
	add_state(
		xr_new<CStateMonsterRest<CChimera> > ("State Rest",
			xr_new<CStateMonsterRestSleep<CChimera> >("StateSleep"), 
			xr_new<CStateMonsterRestWalkGraph<CChimera> >("StateWalkGraph")),
		eStateRest, 1
	);
	
	add_state(
		xr_new<CStateMonsterAttack<CChimera> > ("State Attack",
			xr_new<CStateMonsterAttackRun<CChimera> >("State Attack Run"), 
			xr_new<CStateMonsterAttackMelee<CChimera> >("State Attack Melee")),
		eStateAttack, 1
	);

	add_state(
		xr_new<CStateChimeraThreaten<CChimera> > ("State Threaten",
			xr_new<CStateMonsterThreatenWalk<CChimera> >("State Threaten Walk"), 
			xr_new<CStateMonsterThreatenThreaten<CChimera> >("State Threaten Threaten")),
		eStateThreaten, 1
	);

	add_state(
		xr_new<CStateMonsterPanic<CChimera> > ("State Panic",
			xr_new<CStateMonsterPanicRun<CChimera> >("State Panic Run")), 
		eStatePanic, 1
	);

	add_transition			(eStateRest,	eStateAttack,	1, 1);
	add_transition			(eStateRest,	eStatePanic,	1, 1);
	add_transition			(eStatePanic,	eStateAttack,	1, 1);

	inherited::Load			(section);
}

void CStateManagerChimera::reinit			(CChimera *object)
{
	inherited::reinit		(object);

	set_current_state		(eStateRest);
	set_dest_state			(eStateRest);
}


void CStateManagerChimera::initialize		()
{
	inherited::initialize	();
	
	time_threaten	= 0;
}

void CStateManagerChimera::execute			()
{
	//	m_object->SetUpperState(false);

	


//	VisionElem ve;
//	if (m_object->C)					set_dest_state(eStatePanic);
//	else if (m_object->K)				set_dest_state(eStateAttack);
//	else if (m_object->A)				set_dest_state(eStateDangerousSound);
//	else if (m_object->B)				set_dest_state(eStateInterestingSound);
//	else if (m_object->GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((m_object->GetSatiety() < m_object->_sd->m_fMinSatiety) || m_object->flagEatNow))
//										set_dest_state(eStateEat);
//	else								set_dest_state(eStateRest);
	
//	Msg("Chim State Manager Executed!!! time = [%u] obj = [%u] K = [%u]", Level().timeServer(), m_object->m_tEnemy.obj,m_object->K);
//
//	if (!m_object->K) {
//		int a = 10;
//	}
	if (m_object->K) set_dest_state(eStatePanic);
	else			 set_dest_state(eStateRest);
	
	inherited::execute		();
}

void CStateManagerChimera::finalize			()
{
	inherited::finalize		();
}

