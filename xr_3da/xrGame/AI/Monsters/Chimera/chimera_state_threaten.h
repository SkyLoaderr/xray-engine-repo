#pragma once
#include "../state.h"

template<typename _Object>
class	CStateChimeraThreaten : public CState<_Object> {
protected:
	typedef CState<_Object> inherited;
	typedef CState<_Object>* state_ptr;

	enum {
		eStateWalk			= u32(0),
		eStateFaceEnemy,
		eStateThreaten,
		eStateSteal
	};

	enum {
		eActionThreatenFirst,
		eActionSteal,
		eActionWalk,
		eActionThreatenSecond,
		eActionNone
	} m_action;

public:
						CStateChimeraThreaten	(_Object *obj);
	virtual				~CStateChimeraThreaten	();

	virtual	void		initialize				();

	virtual bool 		check_start_conditions	();
	virtual	void		reselect_state			();
	virtual	void		setup_substates			();
	virtual void 		finalize				();
	virtual void 		critical_finalize		();

			void		select_action			();
};

#include "chimera_state_threaten_inline.h"
