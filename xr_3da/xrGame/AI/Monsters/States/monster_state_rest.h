#pragma once

#include "../state.h"

template<typename _Object>
class	CStateMonsterRest : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum {
		eStateSleep				= u32(0),
		eStateWalkGraphPoint,
	};

public:
						CStateMonsterRest		(_Object *obj);
						CStateMonsterRest		(_Object *obj, state_ptr state_sleep, state_ptr state_walk);
	virtual				~CStateMonsterRest		();

	virtual	void		execute					();
};

#include "monster_state_rest_inline.h"
