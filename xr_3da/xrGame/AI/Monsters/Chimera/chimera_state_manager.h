#pragma once

#include "../../../state_manager_state.h"

class CChimera;

class CStateManagerChimera : public CStateManagerState<CChimera> {
protected:
	typedef CStateManagerState<CChimera> inherited;

	enum EChimeraStates {
		eStateRest = u32(0),
		eStateEat,
		eStateAttack,
		eStatePanic,
		eStateThreaten,

		eStateDummy = u32(-1),
	};

public:
						CStateManagerChimera	(LPCSTR state_name);
	virtual				~CStateManagerChimera	();
			
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CChimera *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};
