#pragma once

#include "../../state.h"

template<typename _Object>
class CStateMonsterFindEnemyLook : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	bool		look_side;
	Fvector		current_point;
	u8			current_side_id;

	enum {
		eMoveToPoint = u32(0),
		eLookAround,
		eTurnAround
	};

public:
						CStateMonsterFindEnemyLook	(_Object *obj, state_ptr state_move, state_ptr state_look_around, state_ptr state_turn);
	virtual				~CStateMonsterFindEnemyLook	();

	virtual	void		reselect_state				();
	virtual bool		check_completion			();

	virtual void		setup_substates				();
};

#include "monster_state_find_enemy_look_inline.h"
