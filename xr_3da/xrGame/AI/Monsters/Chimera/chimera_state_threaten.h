#pragma once

#include "../../state.h"

template<typename _Object>
class	CStateChimeraThreaten : public CState<_Object> {
protected:
	typedef CState<_Object> inherited;
	typedef CState<_Object>* state_ptr;

	enum EAttackStates {
		eStateWalk			= u32(0),
		eStateThreaten,
	};

public:
						CStateChimeraThreaten	(_Object *obj, state_ptr state_walk, state_ptr state_threaten);
	virtual				~CStateChimeraThreaten	();

	virtual	void		execute					();
};

#include "chimera_state_threaten_inline.h"
