#pragma once

#include "../state.h"

template<typename _Object>
class	CStateMonsterPanic : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum EPanicStates {
		eStateRun			= u32(0),
		eStateFaceUnprotectedArea,
	};
	
public:
						CStateMonsterPanic		(_Object *obj);
	virtual				~CStateMonsterPanic		();

	virtual void		initialize				();
	virtual	void		reselect_state			();
	virtual void		check_force_state		();
	virtual void		setup_substates			();
};

#include "monster_state_panic_inline.h"
