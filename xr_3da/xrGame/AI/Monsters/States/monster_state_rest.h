#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class	CStateMonsterRest : public CStateManagerState<_Object> {
protected:
	typedef CStateManagerState<_Object> inherited;

	enum ERestStates {
		eRS_Sleep			= u32(0),
		eRS_WalkGraphPoint,

		eRS_StateNumber
	};

	typedef typename CStateBase<_Object>* SSubStatePtr;
	SSubStatePtr		states[eRS_StateNumber];

public:
						CStateMonsterRest		(LPCSTR state_name, SSubStatePtr state_sleep, SSubStatePtr state_walk);
	virtual				~CStateMonsterRest		();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object);

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_rest_inline.h"
