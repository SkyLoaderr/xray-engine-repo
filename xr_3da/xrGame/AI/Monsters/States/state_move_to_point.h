#pragma once
#include "../../state.h"

struct SStateDataMoveToPoint {
	Fvector		point;
	u32			vertex;
	EAction		action;
	bool		accelerated;
	bool		braking;
	u8			accel_type;
};


template<typename _Object>
class CStateMonsterMoveToPoint : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
	
	SStateDataMoveToPoint data;

public:
						CStateMonsterMoveToPoint	(_Object *obj);
	virtual				~CStateMonsterMoveToPoint	();

	virtual void		initialize					();
	virtual	void		execute						();

	virtual bool		check_completion			();
};

#include "state_move_to_point_inline.h"