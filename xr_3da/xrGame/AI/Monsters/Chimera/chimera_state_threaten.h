#pragma once

#include "../state.h"

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

//////////////////////////////////////////////////////////////////////////

template<typename _Object>
class	CStateChimeraTest : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateChimeraTest	(_Object *obj) : inherited(obj){};
	virtual				~CStateChimeraTest	() {}

	virtual void		initialize			();
	virtual	void		execute				();
};

#include "chimera_state_threaten_inline.h"
