#pragma once

#include "../../state.h"

template<typename _Object>
class CStateMonsterAttackMelee : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateMonsterAttackMelee	(_Object *obj);
	virtual				~CStateMonsterAttackMelee	();

	virtual	void		execute						();
};

#include "monster_state_attack_melee_inline.h"
