#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"

class CPseudoGigant : public CBaseMonster,
					  public CControlledEntity<CPseudoGigant> {
	
	typedef		CBaseMonster	inherited;
	typedef		CControlledEntity<CPseudoGigant>	controlled;

	// step_effector
	struct {
		float time;
		float amplitude;	
		float period_number;
	} step_effector;

public:
					CPseudoGigant				();
	virtual			~CPseudoGigant				();	

	virtual void	Load				(LPCSTR section);

	virtual void	StateSelector		();
	virtual bool	ability_earthquake	() {return true;}
	virtual void	event_on_step		();

};

