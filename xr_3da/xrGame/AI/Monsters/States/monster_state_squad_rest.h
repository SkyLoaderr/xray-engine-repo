#pragma once

#include "../state.h"
#include "../../../ai_space.h"
#include "../../../level_navigation_graph.h"
#include "../../../ai_object_location.h"
#include "state_custom_action.h"
#include "state_move_to_point.h"
#include "../../../restricted_object.h"

template<typename _Object>
class	CStateMonsterSquadRest : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum {
		eStateIdle				= u32(0),
		eStateWalkAroundLeader,
		eStateCheckCover,
		eStateRunAround
	};

	u32		time_next_state_reselect;

public:
						CStateMonsterSquadRest		(_Object *obj);
	virtual				~CStateMonsterSquadRest		();

	virtual void		reselect_state				();
	virtual void		setup_substates				();
};

#include "monster_state_squad_rest_inline.h"