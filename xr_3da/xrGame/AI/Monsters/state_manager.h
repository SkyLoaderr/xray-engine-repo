#pragma once
#include "state_defs.h"

class IStateManagerBase {
public:
	virtual void	reinit					()													= 0;
	virtual void	update					()													= 0;
	virtual void	force_script_state		(EGlobalStates state)	= 0;
	virtual void	execute_script_state	()													= 0;
};
