#pragma once
#include "../../state.h"
#include "state_data.h"

template<typename _Object>
class CStateMonsterMoveToPoint : public CState<_Object> {
	typedef CState<_Object> inherited;
	
	SStateDataMoveToPoint data;

public:
						CStateMonsterMoveToPoint	(_Object *obj) : inherited(obj, ST_MoveToPoint, &data) {}
	virtual				~CStateMonsterMoveToPoint	() {}

	virtual void		initialize					();
	virtual	void		execute						();

	virtual bool		check_completion			();

};


template<typename _Object>
class CStateMonsterMoveToPointEx : public CState<_Object> {
	typedef CState<_Object> inherited;

	SStateDataMoveToPointEx data;
	u32		time_last_build;

public:
						CStateMonsterMoveToPointEx	(_Object *obj) : inherited(obj, ST_MoveToPoint, &data) {}
	virtual				~CStateMonsterMoveToPointEx	() {}
	virtual void		initialize					();
	virtual	void		execute						();
	virtual bool		check_completion			();
};

#include "state_move_to_point_inline.h"
