#pragma once
#include "../state_manager_second.h"

class CAI_Dog;

class CStateManagerDog : public CStateManagerSecond<CAI_Dog> {
	typedef CStateManagerSecond<CAI_Dog> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStateThreaten,
		eStateInterestingSound,
		eStateDangerousSound,
		eStateHitted,
	};

public:

					CStateManagerDog	(CAI_Dog *monster); 
	virtual void	execute				();
};
