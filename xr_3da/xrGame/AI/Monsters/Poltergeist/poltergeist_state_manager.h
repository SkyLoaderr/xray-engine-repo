#pragma once
#include "../state.h"

class CPoltergeist;

class CStateManagerPoltergeist : public CState<CPoltergeist> {
	typedef CState<CPoltergeist> inherited;

	enum {
		eStateRest					 = u32(0),
		eStateEat,
		eStateAttack,
		eStateAttackHidden,
		eStatePanic,
		eStateThreaten,
		eStateDangerousSound,
		eStateInterestingSound,
		eStateHitted,
		eStateFindEnemy,
	};
	

public:
						CStateManagerPoltergeist		(CPoltergeist *obj);
	virtual				~CStateManagerPoltergeist	();

	virtual void		initialize					();
	virtual	void		execute						();

private:

			u32			time_next_flame_attack;
			u32			time_next_tele_attack;
			u32			time_next_scare_attack;

			void		polter_attack				();
			


};
