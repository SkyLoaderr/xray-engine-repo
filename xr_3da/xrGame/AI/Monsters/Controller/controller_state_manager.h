#pragma once
#include "../state_manager_second.h"

class CController;

class CStateManagerController : public CStateManagerSecond<CController> {

	typedef CStateManagerSecond<CController> inherited;

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
