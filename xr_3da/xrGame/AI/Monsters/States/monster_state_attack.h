#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class	CStateMonsterAttack : public CStateManagerState<_Object> {
protected:
	typedef CStateManagerState<_Object> inherited;

	enum EAttackStates {
		eStateRun			= u32(0),
		eStateMelee,

		eStateNumber
	};

	typedef typename CStateBase<_Object>* SSubStatePtr;
	SSubStatePtr		states[eStateNumber];

public:
						CStateMonsterAttack		(LPCSTR state_name, SSubStatePtr state_run, SSubStatePtr state_melee);
	virtual				~CStateMonsterAttack	();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_attack_inline.h"
