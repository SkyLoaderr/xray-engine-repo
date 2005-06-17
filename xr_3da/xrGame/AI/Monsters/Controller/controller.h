#pragma once
#include "../BaseMonster/base_monster.h"
#include "../anim_triple.h"
#include "../../../UIStaticItem.h"
#include "controller_psy_aura.h"
#include "../../../script_export_space.h"
#include "../ai_monster_bones.h"

class CController : public CBaseMonster, 
					public CPsyAuraController {

	typedef		CBaseMonster	inherited;

	u8					m_max_controlled_number;
	ref_sound			control_start_sound;		// звук, который играется в голове у актера
	ref_sound			control_hit_sound;			// звук, который играется в голове у актера


	SAttackEffector		m_control_effector;

	CUIStaticItem		m_UIControlFX;
	CUIStaticItem		m_UIControlFX2;

	u32					time_control_hit_started;
	bool				active_control_fx;
	
	bool				int_need_deactivate;		// internal var

public:
	xr_vector<CEntity*> m_controlled_objects;

public:
					CController			();
	virtual			~CController		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	reinit				();
	virtual void	UpdateCL			();
	virtual void	shedule_Update		(u32 dt);
	virtual void	Die					(CObject* who);

	virtual void	net_Destroy			();
	virtual BOOL	net_Spawn			(CSE_Abstract *DC);

	virtual	void	CheckSpecParams		(u32 spec_params);

	virtual	void	PitchCorrection		() {}

	virtual void	InitThink			();

	virtual bool	ability_can_jump	() {return true;}
			
			void	Jump				();

	//-------------------------------------------------------------------
	// Controller ability
			bool	HasUnderControl		() {return (!m_controlled_objects.empty());}
			void	TakeUnderControl	(CEntity *);
			void	UpdateControlled	();
			void	FreeFromControl		();
			void	OnFreedFromControl	(const CEntity *);  // если монстр сам себя освободил (destroyed || die)

			void	set_controlled_task (u32 task);



			void	play_control_sound_start	();
			void	play_control_sound_hit		();

			void	control_hit					();
	//-------------------------------------------------------------------

public: 
	static	void __stdcall	bone_callback			(CBoneInstance *B);
	void					assign_bones			();
	void					look_direction			(Fvector to_dir, float bone_turn_speed);

	bonesManipulation		m_bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	virtual	const MonsterSpace::SBoneRotation &head_orientation	() const;
	MonsterSpace::SBoneRotation m_head_orient;
	void					update_head_orientation	();

public:
	virtual bool	use_center_to_aim			() const {return true;}

	SAnimationTripleData anim_triple_control;

#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();
#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CController)
#undef script_type_list
#define script_type_list save_type_list(CController)

