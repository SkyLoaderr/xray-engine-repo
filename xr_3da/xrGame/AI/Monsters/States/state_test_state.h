#pragma once
#include "../state.h"

template<typename _Object>
class CStateMonsterTestState : public CState<_Object> {
	typedef CState<_Object> inherited;
	typedef CState<_Object> *state_ptr;

public:
						CStateMonsterTestState	(_Object *obj);
	virtual	void		reselect_state			();
	virtual	void		setup_substates			();
};


#include "state_test_state_inline.h"