////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_manager.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_animation_pair.h"
#include "stalker_animation_script.h"
#include "ai_monster_space.h"
#include "graph_engine_space.h"
#include "CharacterPhysicsSupport.h"
#define CLEAR_STORAGE_ON_CALLBACK

class CMotionDef;
class CBlend;
class CAI_Stalker;
class CWeapon;
class CMissile;
class CPropertyStorage;
class CStalkerAnimationData;

class CStalkerAnimationManager {
public:
	typedef xr_deque<CStalkerAnimationScript>					SCRIPT_ANIMATIONS;
	typedef MonsterSpace::EMovementDirection					EMovementDirection;
	typedef MonsterSpace::EBodyState							EBodyState;
	typedef GraphEngineSpace::_solver_value_type				_value_type;
	typedef GraphEngineSpace::_solver_condition_type			_condition_type;

private:
	const CStalkerAnimationData		*m_data_storage;
	SCRIPT_ANIMATIONS				m_script_animations;

private:
	CStalkerAnimationPair			m_global;
	CStalkerAnimationPair			m_head;
	CStalkerAnimationPair			m_torso;
	CStalkerAnimationPair			m_legs;
	CStalkerAnimationPair			m_script;

private:
	u32								m_direction_start;
	EMovementDirection				m_current_direction;
	EMovementDirection				m_target_direction;

private:
	EMovementDirection				m_previous_speed_direction;
	u32								m_change_direction_time;
	mutable int						m_looking_back;

private:
	int								m_crouch_state;
	bool							m_no_move_actual;

private:
	CAI_Stalker						*m_object;
	IRender_Visual					*m_visual;
	CKinematicsAnimated				*m_skeleton_animated;

private:
	CWeapon							*m_weapon;
	CMissile						*m_missile;

private:
	CPropertyStorage				*m_storage;
	_condition_type					m_property_id;
	_value_type						m_property_value;

private:
	bool							m_call_script_callback;

#ifdef USE_HEAD_BONE_PART_FAKE
private:
	u32								m_script_bone_part_mask;
#endif

private:
	float							m_previous_speed;
	float							m_current_speed;

private:
	IC		bool					strapped				() const;
	IC		bool					standing				() const;
	IC		void					fill_object_info		();
	IC		u32						object_slot				() const;
	IC		EBodyState				body_state				() const;

private:
			bool					need_look_back			() const;

private:
			MotionID				no_object_animation		(const EBodyState &body_state) const;
			MotionID				unknown_object_animation(u32 slot, const EBodyState &body_state) const;
			MotionID				weapon_animation		(u32 slot, const EBodyState &body_state) const;
			MotionID				missile_animation		(u32 slot, const EBodyState &body_state) const;

private:
	IC		float					legs_switch_factor		() const;
			void					legs_assign_direction	(float switch_factor, const EMovementDirection &direction);
			void					legs_process_direction	(float yaw);
			MotionID				legs_move_animation		();
			MotionID				legs_no_move_animation	();

protected:
			void					assign_bone_callbacks	();
			MotionID				assign_global_animation	();
			MotionID				assign_head_animation	();
			MotionID				assign_torso_animation	();
			MotionID				assign_legs_animation	();
			MotionID				assign_script_animation	();

protected:
	static	void					global_play_callback	(CBlend *blend);
	static	void					head_play_callback		(CBlend *blend);
	static	void					torso_play_callback		(CBlend *blend);
	static	void					legs_play_callback		(CBlend *blend);
	static	void					script_play_callback	(CBlend *blend);

public:
	virtual	void					reinit					();
	virtual	void					reload					(CAI_Stalker *object);
	virtual void					update					();
			void					play_delayed_callbacks	();
			void					play_fx					(float power_factor, int fx_index);

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
};

#include "stalker_animation_manager_inline.h"