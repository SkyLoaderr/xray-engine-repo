#pragma once
#include "../state_manager_second.h"

class CPseudoGigant;

class CStateManagerGigant : public CStateManagerSecond<CPseudoGigant> {
	typedef CStateManagerSecond<CPseudoGigant> inherited;
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

					CStateManagerGigant	(CPseudoGigant *monster); 
	virtual void	execute				();
};
