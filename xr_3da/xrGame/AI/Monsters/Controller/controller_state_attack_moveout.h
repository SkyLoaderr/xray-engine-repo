#pragma once

template<typename _Object>
class CStateControlMoveOut : public CState<_Object> {
	typedef	CState<_Object>		inherited;
	typedef	CState<_Object>*	state_ptr;

public:

					CStateControlMoveOut	(_Object *obj) : inherited(obj) {}
	virtual			~CStateControlMoveOut	() {}

	virtual void	execute					();
	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();

};

#include "controller_state_attack_moveout_inline.h"

