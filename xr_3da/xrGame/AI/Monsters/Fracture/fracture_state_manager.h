#pragma once
#include "../state.h"

class CFracture;

class CStateManagerFracture : public CState<CFracture> {
	typedef CState<CFracture> inherited;

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
