#pragma once

#include "../../state.h"

template<typename _Object>
class	CStateMonsterEat : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	enum EStates{
		eStateCorpseApproachRun		= u32(0),
		eStateCorpseApproachWalk,
		eStateCheckCorpse,
		eStateEat,
		eStateWalkAway,
		eStateRest
	};

public:
						CStateMonsterEat		(_Object *obj, state_ptr state_eat);
	virtual				~CStateMonsterEat		();

	virtual void		initialize				();
	virtual	void		reselect				();
	virtual	void		setup_substates			();
};

#include "monster_state_eat_inline.h"
