#pragma once
#include "../state_manager_second.h"

class CAI_Flesh;

class CStateManagerFlesh : public CStateManagerSecond<CAI_Flesh> {
	typedef CStateManagerSecond<CAI_Flesh> inherited;

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

					CStateManagerFlesh	(CAI_Flesh *monster); 
	virtual void	execute				();
};
