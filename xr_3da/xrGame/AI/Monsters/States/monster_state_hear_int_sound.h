#pragma once

#include "../state.h"

template<typename _Object>
class	CStateMonsterHearInterestingSound : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum {
		eStateMoveToDest		= u32(0),
		eStateLookAround,
	};

public:
					CStateMonsterHearInterestingSound	(_Object *obj);
	virtual			~CStateMonsterHearInterestingSound	(){}

	virtual void	reselect_state						();
	virtual void	setup_substates						();

};

#include "monster_state_hear_int_sound_inline.h"