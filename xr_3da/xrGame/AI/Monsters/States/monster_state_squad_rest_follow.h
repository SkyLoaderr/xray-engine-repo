#pragma once

#include "../state.h"
#include "state_custom_action.h"
#include "state_move_to_point.h"

template<typename _Object>
class	CStateMonsterSquadRestFollow : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum {
		eStateIdle				= u32(0),
		eStateWalkToPoint,
	};

	Fvector		last_point;

public:
						CStateMonsterSquadRestFollow	(_Object *obj);
	virtual				~CStateMonsterSquadRestFollow	();

	virtual void		initialize						();
	virtual void		reselect_state					();
	virtual void		setup_substates					();
	virtual void		check_force_state				();
};

#include "monster_state_squad_rest_follow_inline.h"
