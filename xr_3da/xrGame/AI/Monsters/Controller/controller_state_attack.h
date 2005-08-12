#pragma once

#include "../state.h"
#include "../../../ai_debug.h"

template<typename _Object>
class	CStateControllerAttack : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

public:
						CStateControllerAttack	(_Object *obj, state_ptr state_run, state_ptr state_melee);
	virtual				~CStateControllerAttack	() {}

	virtual void		execute					();
	virtual void		setup_substates			();
	virtual void		check_force_state		();
};

#include "controller_state_attack_inline.h"
