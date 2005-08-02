#pragma once

template<typename _Object>
class CStateControlHide : public CState<_Object> {
	typedef	CState<_Object>		inherited;
	typedef	CState<_Object>*	state_ptr;

	bool			m_cover_reached;

	struct {
		Fvector position;
		u32		node;
	} target;

	u32				m_time_finished;

public:

					CStateControlHide		(_Object *obj) : inherited(obj) {}
	virtual			~CStateControlHide		() {}

	virtual void	reinit					();

	virtual void	initialize				();
	virtual void	execute					();

	virtual void	finalize				();

	virtual bool 	check_completion		();
	virtual bool 	check_start_conditions	();


private:
			void	select_target_point		();
};


#include "controller_state_attack_hide_inline.h"

