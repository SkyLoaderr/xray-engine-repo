#pragma once

#include "../../state.h"

template<typename _Object>
class CStateMonsterAttackRun : public CState<_Object> {
	typedef CState<_Object> inherited;

	TTime				m_time_path_rebuild;

public:
						CStateMonsterAttackRun	(_Object *obj);
	virtual				~CStateMonsterAttackRun	();

	virtual void		initialize				();
	virtual	void		execute					();
};

#include "monster_state_attack_run_inline.h"
