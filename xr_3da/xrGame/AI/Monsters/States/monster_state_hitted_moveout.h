#pragma once

template<typename _Object>
class CStateMonsterHittedMoveOut : public CState<_Object> {
	typedef	CState<_Object>		inherited;
	typedef	CState<_Object>*	state_ptr;

	struct {
		Fvector position;
		u32		node;
	} target;

public:

					CStateMonsterHittedMoveOut	(_Object *obj) : inherited(obj) {}
	virtual			~CStateMonsterHittedMoveOut	() {}

	virtual	void	initialize					();
	virtual void	execute						();
	virtual bool 	check_completion			();

private:
			void	select_target				();

};

#include "monster_state_hitted_moveout_inline.h"

