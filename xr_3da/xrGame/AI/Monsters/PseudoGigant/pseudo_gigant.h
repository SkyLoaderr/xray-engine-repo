#pragma once
#include "../../biting/ai_biting.h"
#include "../../controlled_entity.h"

class CPseudoGigant : public CAI_Biting,
					  public CControlledEntity<CPseudoGigant> {
	
	typedef		CAI_Biting	inherited;
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

