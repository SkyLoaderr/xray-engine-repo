////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker velocity manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "movement_manager.h"
#include "ai_monster_space.h"

using namespace MonsterSpace;

class CAI_Stalker;

class CStalkerMovementManager : virtual public CMovementManager {
public:
	struct CMovementParams {
		Fvector						m_desired_position;
		Fvector						m_desired_direction;
		EBodyState					m_body_state;
		EMovementType				m_movement_type;
		EMentalState				m_mental_state;
		EPathType					m_path_type;
		EDetailPathType				m_detail_path_type;
		CAbstractVertexEvaluator	*m_node_evaluator;
		CAbstractVertexEvaluator	*m_path_evaluator;
		bool						m_use_desired_position;
		bool						m_use_desired_direction;
	};

protected:
	typedef CMovementManager inherited;

protected:
	float								m_fCrouchFactor;
	float								m_fWalkFactor;
	float								m_fWalkBackFactor;
	float								m_fRunFactor;
	float								m_fRunBackFactor;
	float								m_fWalkFreeFactor;
	float								m_fRunFreeFactor;
	float								m_fPanicFactor;
	float								m_fDamagedWalkFactor;
	float								m_fDamagedRunFactor;
	float								m_fDamagedWalkFreeFactor;
	float								m_fDamagedRunFreeFactor;
	float								m_fDamagedPanicFactor;

protected:
	CMovementParams						m_current;
	CMovementParams						m_target;
	CAI_Stalker							*m_stalker;

public:
	MonsterSpace::SBoneRotation			m_head;
	u32									m_last_turn_index;

protected:
	IC		void	setup_head_speed		();
	IC		void	add_velocity			(int mask, float linear, float compute_angular, float angular);
	IC		void	add_velocity			(int mask, float linear, float compute_angular);
	IC		void	setup_body_orientation	();
			void	init_velocity_masks		();
			void	setup_movement_params	();
			bool	script_control			();
			void	setup_velocities		();
			void	parse_velocity_mask		();

public:
					CStalkerMovementManager	();
	virtual			~CStalkerMovementManager();
			void	init					();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);
	virtual	void	update					(u32 time_delta);
	IC		float	path_direction_angle	();
	IC		bool	turn_in_place			() const;

	IC		void	set_head_orientation	(const MonsterSpace::SBoneRotation &orientation);
			void	set_desired_position	(const Fvector *desired_position);
	IC		void	set_desired_direction	(const Fvector *desired_direction);
	IC		void	set_body_state			(EBodyState body_state);
	IC		void	set_movement_type		(EMovementType movement_type);
	IC		void	set_mental_state		(EMentalState mental_state);
	IC		void	set_path_type			(EPathType path_type);
	IC		void	set_detail_path_type	(EDetailPathType detail_path_type);
	IC		void	set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator);
	IC		void	set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator);

	IC		const MonsterSpace::SBoneRotation		&head_orientation		() const;
	IC		const Fvector							&desired_position		() const;
	IC		const Fvector							&desired_direction		() const;
	IC		const MonsterSpace::EBodyState			body_state				() const;
	IC		const MonsterSpace::EMovementType		movement_type			() const;
	IC		const MonsterSpace::EMentalState		mental_state			() const;
	IC		const EPathType							path_type				() const;
	IC		const EDetailPathType					detail_path_type		() const;
	IC		CAbstractVertexEvaluator				*node_evaluator			() const;
	IC		CAbstractVertexEvaluator				*path_evaluator			() const;
	IC		bool									use_desired_position	() const;
	IC		bool									use_desired_direction	() const;
};

#include "stalker_movement_manager_inline.h"