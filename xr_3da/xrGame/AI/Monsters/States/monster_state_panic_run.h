#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterPanicRun : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateMonsterPanicRun	(LPCSTR state_name);
	virtual				~CStateMonsterPanicRun	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_panic_run_inline.h"
