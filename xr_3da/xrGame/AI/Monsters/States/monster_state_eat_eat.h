#pragma once
#include "../state.h"

template<typename _Object>
class CStateMonsterEating : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;

	enum {
		eStateEat		= u32(0),
		eStateWalkCloser,
	}cur_state, prev_state;

	CEntityAlive	*corpse;
	u32				time_last_eat;

public:
						CStateMonsterEating		(_Object *obj);
	virtual				~CStateMonsterEating	();

	virtual void		initialize				();
	virtual	void		execute					();

	virtual bool		check_completion		();
};

#include "monster_state_eat_eat_inline.h"
