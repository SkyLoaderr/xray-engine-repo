#pragma once
#include "../state_manager_second.h"

class CBurer;

class CStateManagerBurer : public CStateManagerSecond<CBurer> {
	typedef CStateManagerSecond<CBurer> inherited;
public:
					CStateManagerBurer		(CBurer *monster); 
	virtual void	execute					();
	virtual void	setup_substates			();
};

