#pragma once
#include "../state.h"

class CController;

class CStateManagerController : public CState<CController> {

	typedef CState<CController> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStatePanic,
		eStateThreaten,
		eStateDangerousSound,
		eStateInterestingSound,
		eStateHitted,
		eStateFindEnemy,
	};

public:
						CStateManagerController		(CController *obj);
	virtual				~CStateManagerController	();

	virtual	void		execute						();
};
