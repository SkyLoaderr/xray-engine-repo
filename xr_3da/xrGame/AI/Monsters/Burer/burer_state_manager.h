#pragma once

#include "../../../state_manager_state.h"

class CBurer;

class CStateManagerBurer : public CStateManagerState<CBurer> {
protected:
	typedef CStateManagerState<CBurer> inherited;

	enum EBurerStates {
		eStateRest = u32(0),
		eStateEat,
		eStateAttack,
		eStatePanic,
		eStateDummy = u32(-1),
	};

public:
						CStateManagerBurer		(LPCSTR state_name);
	virtual				~CStateManagerBurer		();

	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CBurer *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};
