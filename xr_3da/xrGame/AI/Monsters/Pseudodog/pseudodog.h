#pragma once

#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"
#include "../control_jump.h"

class CAI_PseudoDog : public CBaseMonster {
	typedef		CBaseMonster	inherited;

	CControlJump		m_jump;

public:

	SAttackEffector m_psi_effector;

	float			m_anger_hunger_threshold;
	float			m_anger_loud_threshold;

	TTime			m_time_became_angry;

	TTime			time_growling;			// время нахождения в состоянии пугания

	ref_sound		psy_effect_sound;		// звук, который играется в голове у актера
	float			psy_effect_turn_angle;

public:
					CAI_PseudoDog		();
	virtual			~CAI_PseudoDog		();	

	virtual void	Load				(LPCSTR section);

	virtual void	reinit				();
	virtual void	reload				(LPCSTR section);

	virtual bool	ability_can_drag	() {return true;}
	virtual bool	ability_psi_attack	() {return true;}
	virtual bool	ability_can_jump	() {return true;}

	virtual void	CheckSpecParams		(u32 spec_params);
	//virtual void	play_effect_sound	();

	virtual bool	ability_jump_over_physics	() {return true;}
	virtual void	jump_over_physics			(const Fvector &target);
	virtual bool	jump						(CObject *enemy);

			void	try_to_jump			();
		
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_PseudoDog)
#undef script_type_list
#define script_type_list save_type_list(CAI_PseudoDog)
