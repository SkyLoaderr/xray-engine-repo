#pragma once

template<typename _Object>
class CStateControlAttack : public CState<_Object> {
	typedef	CState<_Object> inherited;

public:

					CStateControlAttack		(_Object *p);
	virtual			~CStateControlAttack	();

	virtual void	execute					();

	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();
};

#include "controller_state_control_hit_inline.h"