#pragma once
#include "../../state.h"

class CZombie;

class CStateManagerZombie : public CState<CZombie> {
	typedef CState<CZombie> inherited;

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
	};

public:
						CStateManagerZombie		(CZombie *obj);
	virtual				~CStateManagerZombie	();

	virtual void		initialize				();
	virtual	void		execute					();
	
			void		squad_notify			();
	
};
