#pragma once
#include "../state.h"

template<typename _Object>
class	CStateBloodsuckerVampire : public CState<_Object> {
	typedef CState<_Object>		inherited;

	enum {
		eActionPrepare,
		eActionContinue,
		eActionFire,
		eActionWaitTripleEnd,
		eActionCompleted
	} m_action;

	u32					time_vampire_started;

public:
						CStateBloodsuckerVampire	(_Object *obj);
	virtual				~CStateBloodsuckerVampire	();

	virtual void		initialize					();
	virtual	void		execute						();
	virtual	void		finalize					();
	virtual	void		critical_finalize			();
	virtual bool		check_start_conditions		();
	virtual bool		check_completion			();

private:
			void		execute_vampire_prepare		();
			void		execute_vampire_continue	();
			void		execute_vampire_hit			();
};

#include "bloodsucker_vampire_inline.h"
