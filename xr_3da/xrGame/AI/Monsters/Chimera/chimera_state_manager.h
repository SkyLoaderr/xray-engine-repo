#pragma once
#include "../state_manager_second.h"

class CChimera;

class CStateManagerChimera : public CStateManagerSecond<CChimera> {
	
	typedef CStateManagerSecond<CChimera> inherited;

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
	};

public:
						CStateManagerChimera	(CChimera *obj);
	virtual				~CStateManagerChimera	();

	virtual	void		execute					();
};
