#pragma once
#include "../state.h"

template<typename _Object>
class CStateBloodsuckerVampireHide : public CState<_Object> {
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

public:
						CStateBloodsuckerVampireHide	(_Object *obj);

	//virtual void		initialize						();
	//virtual	void		reselect_state					();
	//virtual	void		finalize						();
	//virtual	void		critical_finalize				();
	//virtual bool		check_completion				();

	//virtual void		setup_substates					();
};

#include "bloodsucker_vampire_hide_inline.h"
