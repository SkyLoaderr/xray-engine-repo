#pragma once

#include "../../state.h"

template<typename _Object>
class	CStateControllerAttack : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum EAttackStates {
		eStateRun			= u32(0),
		eStateMelee,
		eStateControlAttack,
		eStateControlExpose
	};

public:
						CStateControllerAttack	(_Object *obj, state_ptr state_run, state_ptr state_melee);
	virtual				~CStateControllerAttack	();

	virtual void		reselect_state			();
};

#include "controller_state_attack_inline.h"
