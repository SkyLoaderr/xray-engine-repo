#pragma once

template<typename _Object>
class CStateControlExpose : public CState<_Object> {
	typedef	CState<_Object> inherited;

public:

					CStateControlExpose		(_Object *p);
	virtual			~CStateControlExpose	();

	virtual void	execute					();
	virtual void	finalize				();
	virtual void	critical_finalize		();

	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();
};

#include "controller_state_expose_inline.h"