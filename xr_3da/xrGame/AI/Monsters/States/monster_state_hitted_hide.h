#pragma once

template<typename _Object>
class CStateMonsterHittedHide : public CState<_Object> {
	typedef	CState<_Object>		inherited;
	typedef	CState<_Object>*	state_ptr;

	bool			m_cover_reached;

	struct {
		Fvector position;
		u32		node;
	} target;

	Fvector			danger_point;

public:

					CStateMonsterHittedHide	(_Object *obj) : inherited(obj) {}
	virtual			~CStateMonsterHittedHide() {}

	virtual void	initialize				();
	virtual void	execute					();

	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();

private:
			void	select_target_point		();
			
};

#include "monster_state_hitted_hide_inline.h"

