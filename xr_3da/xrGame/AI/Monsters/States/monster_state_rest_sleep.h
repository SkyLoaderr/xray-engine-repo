#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterRestSleep : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateMonsterRestSleep	(LPCSTR state_name);
	virtual				~CStateMonsterRestSleep	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_rest_sleep_inline.h"
