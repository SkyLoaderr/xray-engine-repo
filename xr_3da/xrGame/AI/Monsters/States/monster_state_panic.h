#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class	CStateMonsterPanic : public CStateManagerState<_Object> {
protected:
	typedef CStateManagerState<_Object> inherited;

	enum EAttackStates {
		eStateRun			= u32(0),

		eStateNumber
	};

	typedef typename CStateBase<_Object>* SSubStatePtr;
	SSubStatePtr		states[eStateNumber];

public:
						CStateMonsterPanic		(LPCSTR state_name, SSubStatePtr state_run);
	virtual				~CStateMonsterPanic		();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

	virtual bool		completed				();
};

#include "monster_state_panic_inline.h"

