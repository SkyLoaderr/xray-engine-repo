////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_manager.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_animation_state.h"
#include "stalker_animation_names.h"
#include "stalker_animation_pair.h"
#include "stalker_animation_script.h"
#include "ai_monster_space.h"

class CMotionDef;
class CBlend;
class CAI_Stalker;
class CWeapon;
class CMissile;

class CStalkerAnimationManager {
public:
	typedef CStalkerAnimationState::WEAPON_ACTIONS				WEAPON_ACTIONS;
	typedef CAniCollection<WEAPON_ACTIONS,weapon_names>			GLOBAL_ANIMATIONS;
	typedef CAniCollection<CStalkerAnimationState,state_names>	PART_ANIMATIONS;
	typedef CAniFVector<head_names>								HEAD_ANIMATIONS;
	typedef xr_deque<CStalkerAnimationScript>					SCRIPT_ANIMATIONS;
	typedef MonsterSpace::EMovementDirection					EMovementDirection;
	typedef MonsterSpace::EBodyState							EBodyState;

public:
	PART_ANIMATIONS			m_part_animations;
	HEAD_ANIMATIONS			m_head_animations;
	GLOBAL_ANIMATIONS		m_global_animations;
	SCRIPT_ANIMATIONS		m_script_animations;

private:
	CStalkerAnimationPair	m_global;
	CStalkerAnimationPair	m_head;
	CStalkerAnimationPair	m_torso;
	CStalkerAnimationPair	m_legs;
	CStalkerAnimationPair	m_script;

private:
	u8						m_animation_index;
	u32						m_direction_start_time;
	u32						m_animation_switch_interval;
	EMovementDirection		m_movement_direction;
	EMovementDirection		m_desirable_direction;

private:
	CAI_Stalker				*m_object;
	IRender_Visual			*m_visual;
	CSkeletonAnimated		*m_skeleton_animated;

private:
	CWeapon					*m_weapon;
	CMissile				*m_missile;

protected:
			void			fill_object_info				();
			EBodyState		body_state						() const;
			u32				object_slot						() const;
			CMotionDef		*no_object_animation			(const EBodyState &body_state) const;
			CMotionDef		*unknown_object_animation		(u32 slot, const EBodyState &body_state) const;
			CMotionDef		*weapon_animation				(u32 slot, const EBodyState &body_state) const;
			CMotionDef		*missile_animation				(u32 slot, const EBodyState &body_state) const;

protected:
			void			assign_bone_callbacks			();
			CMotionDef 		*assign_global_animation		();
			CMotionDef 		*assign_head_animation			();
			CMotionDef 		*assign_torso_animation			();
			CMotionDef		*assign_legs_animation			();
			CMotionDef		*assign_script_animation			();

protected:
	static void				global_play_callback			(CBlend *blend);
	static void				head_play_callback				(CBlend *blend);
	static void				torso_play_callback				(CBlend *blend);
	static void				legs_play_callback				(CBlend *blend);
	static void				script_play_callback			(CBlend *blend);

public:
	virtual	void			reinit							();
	virtual	void			reload							(CAI_Stalker *object);
	virtual void			update							();
			void			play_fx							(float power_factor, int fx_index);

public:
			void					add_script_animation	(LPCSTR animation, bool hand_usage = false);
	IC		void					clear_script_animations	();
	IC		void					pop_script_animation	();
	IC		const SCRIPT_ANIMATIONS	&script_animations		() const;

public:
	IC		CStalkerAnimationPair	&global					();
	IC		CStalkerAnimationPair	&head					();
	IC		CStalkerAnimationPair	&torso					();
	IC		CStalkerAnimationPair	&legs					();
	IC		CStalkerAnimationPair	&script					();
	IC		CAI_Stalker				*object					();
	IC		const CAI_Stalker		*object					() const;
};

#include "stalker_animation_manager_inline.h"