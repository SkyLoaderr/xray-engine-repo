#pragma once

#include "../state.h"
#include "../../../ai_debug.h"

template<typename _Object>
class CStateMonsterRestIdle : public CState<_Object> {
	typedef CState<_Object> inherited;
public:
						CStateMonsterRestIdle	(_Object *obj);
	virtual	void		execute					();
};

#include "monster_state_rest_idle_inline.h"
