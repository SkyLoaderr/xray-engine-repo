#pragma once
#include "../state_manager_second.h"

class CAI_Bloodsucker;

class CStateManagerBloodsucker : public CStateManagerSecond<CAI_Bloodsucker> {
	typedef CStateManagerSecond<CAI_Bloodsucker> inherited;

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

					CStateManagerBloodsucker	(CAI_Bloodsucker *monster); 

	virtual void	execute						();
};
