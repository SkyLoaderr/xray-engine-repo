#pragma once

#include "../state.h"


template<typename _Object>
class	CStateMonsterFindEnemy : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum EAttackStates {
		eStateRun			= u32(0),
		eStateLookAround,
		eStateAngry,
		eStateWalkAround
	};


public:
						CStateMonsterFindEnemy	(_Object *obj, state_ptr state_run, state_ptr state_look_around, state_ptr state_angry, state_ptr state_walk_around);
	virtual				~CStateMonsterFindEnemy	();

	virtual	void		reselect_state			();
};

#include "monster_state_find_enemy_inline.h"

