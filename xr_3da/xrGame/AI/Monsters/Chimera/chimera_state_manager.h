#pragma once
#include "../state.h"

class CChimera;

class CStateManagerChimera : public CState<CChimera> {
	
	typedef CState<CChimera> inherited;

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
