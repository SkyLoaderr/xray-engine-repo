#pragma once

#include "../state.h"

template<typename _Object>
class CStateMonsterRestSleep : public CState<_Object> {
	typedef CState<_Object> inherited;
public:
						CStateMonsterRestSleep	(_Object *obj);
	virtual				~CStateMonsterRestSleep	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
	virtual	void		critical_finalize		();
};

#include "monster_state_rest_sleep_inline.h"
