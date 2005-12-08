#pragma once
#include "../state.h"

template<typename _Object>
class	CStateBloodsuckerPredator : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	u32							m_target_node;

public:
						CStateBloodsuckerPredator		(_Object *obj);

	virtual void		reinit							();

	virtual void		initialize						();
	virtual	void		reselect_state					();
	virtual	void		finalize						();
	virtual	void		critical_finalize				();
	virtual bool		check_start_conditions			();
	virtual bool		check_completion				();

	virtual void		setup_substates					();
};

#include "bloodsucker_predator_inline.h"