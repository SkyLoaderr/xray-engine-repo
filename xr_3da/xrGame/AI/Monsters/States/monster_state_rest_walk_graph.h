#pragma once

#include "../../../state_manager_state.h"

template<typename _Object>
class CStateMonsterRestWalkGraph : public CStateBase<_Object> {
	typedef CStateBase<_Object> inherited;

public:
						CStateMonsterRestWalkGraph	(LPCSTR state_name);
	virtual				~CStateMonsterRestWalkGraph	();

	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "monster_state_rest_walk_graph_inline.h"
