////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "movement_manager.h"
#include "ai_monster_space.h"
#include "stalker_movement_params.h"

using namespace MonsterSpace;

class CAI_Stalker;
class CStalkerVelocityCollection;

//#define OLD_VELOCITIES

class CStalkerMovementManager : public CMovementManager {
protected:
	typedef CMovementManager					inherited;

public:
	typedef DetailPathManager::EDetailPathType	EDetailPathType;

#ifndef OLD_VELOCITIES
protected:
	const CStalkerVelocityCollection	*m_velocities;
#else
protected:
	float								m_crouch_factor;
	float								m_walk_factor;
	float								m_walk_back_factor;
	float								m_run_factor;
	float								m_run_back_factor;
	float								m_walk_free_factor;
	float								m_run_free_factor;
	float								m_panic_factor;
	float								m_damaged_walk_factor;
	float								m_damaged_run_factor;
	float								m_damaged_walk_free_factor;
	float								m_damaged_run_free_factor;
	float								m_damaged_panic_factor;
#endif

protected:
	CStalkerMovementParams				m_current;
	CStalkerMovementParams				m_target;
	CAI_Stalker							*m_object;

public:
	MonsterSpace::SBoneRotation			m_head;
	u32									m_last_turn_index;

protected:
	IC		void	setup_head_speed				();
	IC		void	add_velocity					(int mask, float linear, float compute_angular, float angular);
	IC		void	add_velocity					(int mask, float linear, float compute_angular);
	IC		void	setup_body_orientation			();
			void	init_velocity_masks				();
			void	setup_movement_params			();
			bool	script_control					();
			void	setup_velocities				();
			void	parse_velocity_mask				();

public:
					CStalkerMovementManager			(CAI_Stalker *object);
	virtual			~CStalkerMovementManager		();
	virtual	void	Load							(LPCSTR section);
	virtual	void	reinit							();
	virtual	void	reload							(LPCSTR section);
	virtual	void	update							(u32 time_delta);
	virtual void	on_travel_point_change			(const u32 &previous_travel_point_index);
	virtual	void	on_restrictions_change			();
			void	initialize						();
	IC		float	path_direction_angle			();
	IC		bool	turn_in_place					() const;

	IC		void	set_head_orientation			(const MonsterSpace::SBoneRotation &orientation);
			void	set_desired_position			(const Fvector *desired_position);
	IC		void	set_desired_direction			(const Fvector *desired_direction);
	IC		void	set_body_state					(EBodyState body_state);
	IC		void	set_movement_type				(EMovementType movement_type);
	IC		void	set_mental_state				(EMentalState mental_state);
	IC		void	set_path_type					(EPathType path_type);
	IC		void	set_detail_path_type			(EDetailPathType detail_path_type);
			void	set_nearest_accessible_position	();
			void	set_nearest_accessible_position	(Fvector desired_position, u32 level_vertex_id);
			void	adjust_speed_to_animation		(const MonsterSpace::EMovementDirection &movement_direction);

#ifdef OLD_VELOCITIES
public:
	IC		float	crouch_factor					() const;
	IC		float	walk_factor						() const;
	IC		float	walk_back_factor				() const;
	IC		float	run_factor						() const;
	IC		float	run_back_factor					() const;
	IC		float	walk_free_factor				() const;
	IC		float	run_free_factor					() const;
	IC		float	panic_factor					() const;
	IC		float	damaged_walk_factor				() const;
	IC		float	damaged_run_factor				() const;
	IC		float	damaged_walk_free_factor		() const;
	IC		float	damaged_run_free_factor			() const;
	IC		float	damaged_panic_factor			() const;
#endif

public:
	IC		const MonsterSpace::SBoneRotation		&head_orientation		() const;
	IC		const Fvector							&desired_position		() const;
	IC		const Fvector							&desired_direction		() const;
	IC		const MonsterSpace::EBodyState			body_state				() const;
	IC		const MonsterSpace::EBodyState			target_body_state		() const;
	IC		const MonsterSpace::EMovementType		movement_type			() const;
	IC		const MonsterSpace::EMentalState		mental_state			() const;
	IC		const MonsterSpace::EMentalState		target_mental_state		() const;
	IC		const EPathType							path_type				() const;
	IC		const EDetailPathType					detail_path_type		() const;
	IC		bool									use_desired_position	() const;
	IC		bool									use_desired_direction	() const;

public:
	IC		const MonsterSpace::EMovementType		target_movement_type	() const;

public:
	IC		CAI_Stalker								&object					() const;
};

#include "stalker_movement_manager_inline.h"