#pragma once
#include "../../biting/ai_biting.h"

class CPseudoGigant : public CAI_Biting {
	typedef		CAI_Biting	inherited;

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
	virtual	void	ProcessTurn			();
	virtual u8		get_legs_number		() {return BIPEDAL;}

	virtual bool	ability_earthquake	() {return true;}
	virtual void	event_on_step		();

};

