#pragma once

template<typename _Object>
class CStateControlMoveOut : public CState<_Object> {
	typedef	CState<_Object>		inherited;
	typedef	CState<_Object>*	state_ptr;

	u32				m_time_started;

public:

					CStateControlMoveOut	(_Object *obj) : inherited(obj) {}
	virtual			~CStateControlMoveOut	() {}

	virtual void	initialize				();
	virtual void	execute					();
	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();

};

#include "controller_state_attack_moveout_inline.h"

