#pragma once
#include "../state_manager_second.h"

class CSnork;

class CStateManagerSnork : public CStateManagerSecond<CSnork> {
	typedef CStateManagerSecond<CSnork> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStateThreaten,
		eStateInterestingSound,
		eStateDangerousSound,
		eStateHitted,
		eStateFindEnemy,
		eStateFakeDeath,
		eStateSquadRest,
		eStateSquadRestFollow
	};

public:
						CStateManagerSnork		(CSnork *obj);
	virtual				~CStateManagerSnork		();

	virtual	void		execute					();
};
