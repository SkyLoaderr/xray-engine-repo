#pragma once

#include "step_manager_defs.h"

class CCustomMonster;

class CStepManager {
	u8				m_legs_count;

	STEPS_MAP		m_steps_map;
	SStepInfo		m_step_info;

	CCustomMonster	*m_object;

	u16				m_foot_bones[MAX_LEGS_COUNT];
	ref_str			m_animation;

	u32				m_time_anim_started;

public: 
					CStepManager				();
	virtual			~CStepManager				();

	// init on construction
			void	init_external				(CCustomMonster	*obj) {m_object = obj;}
	virtual	void	load						(LPCSTR section);
	virtual void	reinit						();
	
	// call on set animation
	virtual	void	on_animation_start			(ref_str anim);
	// call on updateCL
			void	update						();
	
	// override to return a time of current blend
	virtual float	get_current_animation_time	() {return 1.f;}

	// process event
	virtual	void	event_on_step				() {}	

protected:
			Fvector	get_foot_position			(ELegType leg_type);
private:
			void	reload_foot_bones			();
};
