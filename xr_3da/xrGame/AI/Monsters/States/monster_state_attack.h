#pragma once

#include "../state.h"
#include "../../../ai_debug.h"

template<typename _Object>
class	CStateMonsterAttack : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	u32		m_time_next_run_away;

public:
						CStateMonsterAttack		(_Object *obj);
						CStateMonsterAttack		(_Object *obj, state_ptr state_run, state_ptr state_melee);
	virtual				~CStateMonsterAttack	();
	
	virtual void		initialize				();
	virtual	void		execute					();
	virtual void		setup_substates			();

private:
			bool		check_steal_state		();
			bool		check_find_enemy_state	();
			bool		check_run_away_state	();
			bool		check_run_attack_state	();
			bool		check_camp_state		();
};

#include "monster_state_attack_inline.h"
