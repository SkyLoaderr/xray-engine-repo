#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"

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

	virtual bool	ability_earthquake	() {return true;}
	virtual void	event_on_step		();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPseudoGigant)
#undef script_type_list
#define script_type_list save_type_list(CPseudoGigant)
