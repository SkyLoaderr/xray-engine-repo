#pragma once

#include "step_manager_defs.h"

class CCustomMonster;
class CBlend;

class CStepManager {
	u8				m_legs_count;

	STEPS_MAP		m_steps_map;
	SStepInfo		m_step_info;

	CCustomMonster	*m_object;

	u16				m_foot_bones[MAX_LEGS_COUNT];
	shared_str		m_animation;
	CBlend			*m_blend;

	u32				m_time_anim_started;

public: 
						CStepManager			();
	virtual				~CStepManager			();

	// init on construction
	virtual DLL_Pure	*_construct				();
	virtual	void		load					(LPCSTR section);
	virtual void		reinit					();
	
	// call on set animation
			void		on_animation_start		(shared_str anim, CBlend *blend);
	// call on updateCL
			void		update					();
	
	// process event
	virtual	void		event_on_step			() {}	

protected:
			Fvector		get_foot_position		(ELegType leg_type);
private:
			void		reload_foot_bones		();
			void		load_foot_bones			(CInifile::Sect &data);
			
			float		get_blend_time			();
};
