#pragma once

#include "../state.h"

template<typename _Object>
class	CStateMonsterControlledFollow : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum {
		eStateWait				= u32(0),
		eStateWalkToObject,
	};

public:
						CStateMonsterControlledFollow	(_Object *obj);
	virtual void		reselect_state					();
	virtual void		setup_substates					();
};

#include "monster_state_controlled_follow_inline.h"
