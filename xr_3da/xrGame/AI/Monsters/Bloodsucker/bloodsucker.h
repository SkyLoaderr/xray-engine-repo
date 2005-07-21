#pragma once

#include "../BaseMonster/base_monster.h"
#include "../ai_monster_bones.h"
#include "../invisibility.h"
#include "../controlled_actor.h"
#include "../anim_triple.h"
#include "../../../script_export_space.h"
#include "bloodsucker_alien.h"

class CAI_Bloodsucker : public CBaseMonster, 
						public CInvisibility,
						public CControlledActor {

	typedef		CBaseMonster	inherited;
	
	
	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();
			void			LookDirection			(Fvector to_dir, float bone_turn_speed);


	bonesManipulation		Bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	SMotionVel				invisible_vel;
	LPCSTR					invisible_particle_name;

	LPCSTR					invisible_run_particles_name;
	u32						m_run_particles_freq;
	u32						m_last_invisible_run_play;

public:

	SAnimationTripleData	anim_triple_vampire;

	CBloodsuckerAlien		m_alien_control;
	
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			reinit					();
	virtual	void			reload					(LPCSTR section);

	virtual void			UpdateCL				();
	virtual void			shedule_Update			(u32 dt);

	virtual void			Die						(CObject* who);

	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual void			net_Destroy				();

	virtual	void			Load					(LPCSTR section);
	virtual	void			PitchCorrection			() {}

	virtual	void			CheckSpecParams			(u32 spec_params);
	virtual bool			ability_invisibility	() {return true;}


	virtual	void			post_fsm_update			();

			
	// PP Effector		
	
			void			LoadVampirePPEffector	(LPCSTR section);	
			void			ActivateVampireEffector	(float max_dist);
			
			SPPInfo			pp_vampire_effector;


	virtual	void			on_activate				();
	virtual	void			on_deactivate			();
	virtual	void			on_change_visibility	(bool b_visibility);
	virtual bool			use_center_to_aim		() const {return true;}

	virtual void			UpdateCamera			();

	virtual bool			check_start_conditions	(ControlCom::EControlType);

			void			play_hidden_run_particles	();

#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();
#endif


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_Bloodsucker)
#undef script_type_list
#define script_type_list save_type_list(CAI_Bloodsucker)