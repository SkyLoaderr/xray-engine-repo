#pragma once
#include "../state_manager_second.h"

class CZombie;

class CStateManagerZombie : public CStateManagerSecond<CZombie> {
	typedef CStateManagerSecond<CZombie> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStateThreaten,
		eStateInterestingSound,
		eStateHitted,
		eStateFindEnemy,
		eStateFakeDeath,
		eStateSquadRest,
		eStateSquadRestFollow
	};

public:
						CStateManagerZombie		(CZombie *obj);
	virtual				~CStateManagerZombie	();

	virtual void		initialize				();
	virtual	void		execute					();
	
			void		squad_notify			();
	
};
