#pragma once
#include "state_manager.h"
#include "state_defs.h"

class CBaseMonster;
class IState;

class CStateManagerFirst : public IStateManagerBase {
	
	DEFINE_MAP		(EGlobalStates, IState*, STATES_MAP, STATE_MAP_IT);
	STATES_MAP		m_states;

protected:

	CBaseMonster	*m_object;
	EGlobalStates	m_current_state;

public:

					CStateManagerFirst			(CBaseMonster	*monster);
	virtual			~CStateManagerFirst			();

	virtual void	force_script_state			(EGlobalStates state);
	virtual void	execute_script_state		();
	virtual void	reinit						();	

protected:

	virtual	void	add_state					(EGlobalStates id, IState *p_state);
	virtual void	remove_all					();
	virtual void	set_state					(EGlobalStates state);


			IState	*get_state					(EGlobalStates state_id);
};
