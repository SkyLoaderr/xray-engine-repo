#pragma once
#include "../state.h"

class CSnork;

class CStateManagerSnork : public CState<CSnork> {
	typedef CState<CSnork> inherited;

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
						CStateManagerSnork		(CSnork *obj);
	virtual				~CStateManagerSnork		();

	virtual void		initialize				();
	virtual	void		execute					();

			void		squad_notify			();

};
