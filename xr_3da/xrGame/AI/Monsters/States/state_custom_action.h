#pragma once
#include "../../state.h"
#include "state_data.h"

template<typename _Object>
class CStateMonsterCustomAction : public CState<_Object> {
	typedef CState<_Object> inherited;

	SStateDataCustomAction	data;

public:
						CStateMonsterCustomAction	(_Object *obj);
	virtual				~CStateMonsterCustomAction	();

	virtual	void		execute						();
	
};

#include "state_custom_action_inline.h"