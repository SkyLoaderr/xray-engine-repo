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

	u32					m_last_time_threaten;

public:
						CStateChimeraThreaten	(_Object *obj);
	virtual				~CStateChimeraThreaten	();

	virtual	void		initialize				();

	virtual bool 		check_start_conditions	();
	virtual	void		reselect_state			();
	virtual void 		finalize				();
	virtual void 		critical_finalize		();
	//virtual void 		critical_finalize		();
};

#include "chimera_state_threaten_inline.h"
