#pragma once
#include "../../state.h"

struct SStateDataLookToPoint {
	Fvector		point;
	EAction		action;
};


template<typename _Object>
class CStateMonsterLookToPoint : public CState<_Object> {
	typedef CState<_Object> inherited;

public:

	SStateDataLookToPoint data;

public:
						CStateMonsterLookToPoint	(_Object *obj);
	virtual				~CStateMonsterLookToPoint	();

	virtual void		initialize					();
	virtual	void		execute						();

	virtual bool		check_completion			();
};

#include "state_look_to_point_inline.h"