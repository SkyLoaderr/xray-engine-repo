#pragma once
#include "../state_manager_second.h"

class CAI_PseudoDog;

class CStateManagerPseudodog : public CStateManagerSecond<CAI_PseudoDog> {
	typedef CStateManagerSecond<CAI_PseudoDog> inherited;
	
public:

					CStateManagerPseudodog	(CAI_PseudoDog *monster); 
	virtual void	execute					();
};
