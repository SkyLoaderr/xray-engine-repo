#pragma once
#include "state_manager.h"
#include "state.h"

template <typename _Object>
class CStateManagerSecond : public IStateManagerBase, public CState<_Object> {
	typedef CState<_Object> inherited;

public:
					CStateManagerSecond		(_Object *obj) : inherited(obj) {}

	virtual void	reinit					();
	virtual void	update					();
	virtual void	force_script_state		(EGlobalStates state);
	virtual void	execute_script_state	();
};

#include "state_manager_second_inline.h"

