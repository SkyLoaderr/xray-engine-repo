#pragma once

#include "../../../state_manager_state.h"
#include "../../../PhysicsShell.h"

template<typename _Object>
class	CStateBurerAttack : public CStateManagerState<_Object> {
protected:
	typedef CStateManagerState<_Object> inherited;

	enum EAttackStates {
		eStateTelekinetic			= u32(0),
		eStateGraviStart,
		eStateGraviFinish,
		eStateMelee,

		eStateNumber
	};

	typedef typename CStateBase<_Object>* SSubStatePtr;
	SSubStatePtr		states[eStateNumber];

public:
						CStateBurerAttack		(LPCSTR state_name, SSubStatePtr state_tele, SSubStatePtr state_gravi_start, SSubStatePtr state_gravi_finish, SSubStatePtr state_melee);
	virtual				~CStateBurerAttack		();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

private:
			u32			get_number_available_objects(xr_vector<CObject*> &tpObjects);
};

#include "burer_state_attack_inline.h"
