#pragma once

#include "../state.h"
#include "../../../ai_debug.h"

template<typename _Object>
class	CStateMonsterAttack : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum EAttackStates {
		eStateRun			= u32(0),
		eStateMelee,
		eStateRunAttack,
		eStateRunAway,
		eStateFindEnemy
	};

	u32		m_time_next_run_away;

public:
						CStateMonsterAttack		(_Object *obj);
						CStateMonsterAttack		(_Object *obj, state_ptr state_run, state_ptr state_melee);
	virtual				~CStateMonsterAttack	();
	
	virtual void		initialize				();
	virtual	void		execute					();
	virtual void		setup_substates			();
};

#include "monster_state_attack_inline.h"
