#pragma once
#include "../state_manager_second.h"

class CCat;

class CStateManagerCat : public CStateManagerSecond<CCat> {

	typedef CStateManagerSecond<CCat> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStatePanic,
		eStateThreaten,
		eStateDangerousSound,
		eStateInterestingSound,
		eStateFindEnemy,
		eStateHitted,
		eStateAttackRat,
	};

public:
						CStateManagerCat	(CCat *obj);	
	virtual				~CStateManagerCat	();

	virtual	void		execute				();
};
