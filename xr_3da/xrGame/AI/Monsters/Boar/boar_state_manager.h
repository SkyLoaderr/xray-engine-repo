#pragma once
#include "../state_manager_second.h"

class CAI_Boar;

class CStateManagerBoar : public CStateManagerSecond<CAI_Boar> {
	typedef CStateManagerSecond<CAI_Boar> inherited;
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

					CStateManagerBoar	(CAI_Boar *monster); 

	virtual void	execute				();
};
