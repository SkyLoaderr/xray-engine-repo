#pragma once

#include "../state.h"
#include "../../../ai_debug.h"

template<typename _Object>
class	CStateMonsterControlled : public CState<_Object> {
	typedef CState<_Object>		inherited;

	enum EControlledStates {
		eStateFollow		= u32(0),
		eStateAttack,
	};

public:
						CStateMonsterControlled		(_Object *obj);
	virtual	void		execute						();
};

#include "monster_state_controlled_inline.h"
