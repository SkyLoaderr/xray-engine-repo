#pragma once
#include "../state_manager_second.h"

class CPseudoGigant;

class CStateManagerGigant : public CStateManagerSecond<CPseudoGigant> {
	typedef CStateManagerSecond<CPseudoGigant> inherited;
public:

					CStateManagerGigant	(CPseudoGigant *monster); 
	virtual void	execute				();
};
