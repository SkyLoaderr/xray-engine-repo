#pragma once
#include "../state_manager_second.h"

class CFracture;

class CStateManagerFracture : public CStateManagerSecond<CFracture> {
	typedef CStateManagerSecond<CFracture> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStateThreaten,
		eStatePanic,
		eStateInterestingSound,
		eStateDangerousSound,
		eStateHitted,
		eStateFindEnemy
	};

public:
						CStateManagerFracture	(CFracture *obj);
	virtual				~CStateManagerFracture	();

	virtual void		initialize				();
	virtual	void		execute					();
};
