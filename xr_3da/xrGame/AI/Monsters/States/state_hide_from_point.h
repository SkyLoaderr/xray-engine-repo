#pragma once
#include "../../state.h"
#include "state_data.h"

template<typename _Object>
class CStateMonsterHideFromPoint : public CState<_Object> {
	typedef CState<_Object> inherited;

	SStateHideFromPoint data;

	u32					last_time_reselect_cover;
	
	struct {
		Fvector			position;		
		u32				node;
	} target;

public:
						CStateMonsterHideFromPoint	(_Object *obj);
	virtual				~CStateMonsterHideFromPoint	();

	virtual void		initialize					();
	virtual	void		execute						();

	virtual bool		check_completion			();
};

#include "state_hide_from_point_inline.h"