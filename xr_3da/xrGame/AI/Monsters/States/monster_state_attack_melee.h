#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterAttackMelee : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateMonsterAttackMelee	(LPCSTR state_name);
	virtual				~CStateMonsterAttackMelee	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_attack_melee_inline.h"
