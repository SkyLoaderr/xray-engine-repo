#pragma once
#include "../states/monster_state_attack.h"

template<typename _Object>
class	CBloodsuckerStateAttack : public CStateMonsterAttack<_Object> {
	typedef CStateMonsterAttack<_Object> inherited_attack;
public:
					CBloodsuckerStateAttack		(_Object *obj);
	virtual			~CBloodsuckerStateAttack	();

	virtual	void	execute						();
private:
			void	update_invisibility			();
			bool	check_hiding				();
};

#include "bloodsucker_attack_state_inline.h"
