#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterAttackRun : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

	TTime				m_time_path_rebuild;

public:
						CStateMonsterAttackRun	(LPCSTR state_name);
	virtual				~CStateMonsterAttackRun	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_attack_run_inline.h"
