#pragma once

template<typename _Object>
class CStateMonsterPanicRun : public CState<_Object> {
	typedef CState<_Object> inherited;

	bool				m_cover_reached;
	struct {
		Fvector position;
		u32		node;
	} target;

public:
						CStateMonsterPanicRun	(_Object *obj) : inherited(obj) {}
	virtual				~CStateMonsterPanicRun	() {}

	virtual void		initialize				();
	virtual	void		execute					();

	virtual bool		check_completion		();

private:	
			void		select_target_point		();
};

#include "monster_state_panic_run_inline.h"
