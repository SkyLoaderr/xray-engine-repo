#pragma once

#include "../../state.h"

template<typename _Object>
class CStateMonsterFindEnemyLook : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateMonsterFindEnemyLook	(_Object *obj);
	virtual				~CStateMonsterFindEnemyLook	();

	virtual void		initialize					();
	virtual	void		execute						();
	virtual bool		check_completion			();
};

#include "monster_state_find_enemy_look_inline.h"
