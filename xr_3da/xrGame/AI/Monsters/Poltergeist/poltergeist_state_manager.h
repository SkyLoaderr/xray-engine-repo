#pragma once
#include "../../state.h"

class CPoltergeist;

class CStateManagerPoltergeist : public CState<CPoltergeist> {
	typedef CState<CPoltergeist> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStateAttackHidden,
		eStatePanic,
		eStateThreaten,
		eStateDangerousSound,
		eStateInterestingSound,
		eStateHitted,
		eStateFindEnemy,
	};
	


public:
						CStateManagerPoltergeist		(CPoltergeist *obj);
	virtual				~CStateManagerPoltergeist	();

	virtual	void		execute						();
};
