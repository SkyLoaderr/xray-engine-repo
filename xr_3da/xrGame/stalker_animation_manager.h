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
#include "graph_engine_space.h"

class CMotionDef;
class CBlend;
class CAI_Stalker;
class CWeapon;
class CMissile;
class CPropertyStorage;

class CStalkerAnimationManager {
public:
	typedef CStalkerAnimationState::WEAPON_ACTIONS				WEAPON_ACTIONS;
	typedef CAniCollection<WEAPON_ACTIONS,weapon_names>			GLOBAL_ANIMATIONS;
	typedef CAniCollection<CStalkerAnimationState,state_names>	PART_ANIMATIONS;
	typedef CAniFVector<head_names>								HEAD_ANIMATIONS;
	typedef xr_deque<CStalkerAnimationScript>					SCRIPT_ANIMATIONS;
	typedef MonsterSpace::EMovementDirection					EMovementDirection;
	typedef MonsterSpace::EBodyState							EBodyState;
	typedef GraphEngineSpace::_solver_value_type				_value_type;
	typedef GraphEngineSpace::_solver_condition_type			_condition_type;

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

private:
	CPropertyStorage		*m_storage;
	_condition_type			m_property_id;
	_value_type				m_property_value;

private:
	bool					m_call_script_callback;

protected:
			void			fill_object_info				();
			EBodyState		body_state						() const;
			u32				object_slot						() const;
			MotionID		no_object_animation				(const EBodyState &body_state) const;
			MotionID		unknown_object_animation		(u32 slot, const EBodyState &body_state) const;
			MotionID		weapon_animation				(u32 slot, const EBodyState &body_state) const;
			MotionID		missile_animation				(u32 slot, const EBodyState &body_state) const;
			bool			strapped						() const;

protected:
			void			assign_bone_callbacks			();
			MotionID		assign_global_animation			();
			MotionID		assign_head_animation			();
			MotionID		assign_torso_animation			();
			MotionID		assign_legs_animation			();
			MotionID		assign_script_animation			();

protected:
	static	void			global_play_callback			(CBlend *blend);
	static	void			head_play_callback				(CBlend *blend);
	static	void			torso_play_callback				(CBlend *blend);
	static	void			legs_play_callback				(CBlend *blend);
	static	void			script_play_callback			(CBlend *blend);

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
	IC		CAI_Stalker				&object					() const;

public:
	IC		void					setup_storage			(CPropertyStorage *storage);
	IC		CPropertyStorage		*setup_storage			() const;
	IC		_condition_type			property_id				() const;
	IC		void					property_id				(_condition_type value);
	IC		_value_type				property_value			() const;
	IC		void					property_value			(_value_type value);
	
public:
//	IC		const CAnimationPair	*animation				() const;
};

#include "stalker_animation_manager_inline.h"