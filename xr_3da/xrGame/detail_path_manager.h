////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"

class CDetailPathManager {
public:
	enum EMovementParameters {
		eMovementParameterStand		= u32(1) << 1,
		eMovementParameterJumpUp	= u32(1) << 2,
		eMovementParameterJumpDown	= u32(1) << 3,
		eMovementParameterWalk		= u32(1) << 4,
		eMovementParameterRun		= u32(1) << 5,
		eMovementParameterPanic		= u32(1) << 6,
	};

	enum EDetailPathType {
		eDetailPathTypeSmooth,
		eDetailPathTypeSmoothDodge,
		eDetailPathTypeSmoothCriteria,
	};

	struct SMovementParams {
		float									m_linear_speed;
		float									m_angular_speed;
	};

	struct STravelPoint : public SMovementParams {
		Fvector									m_position;
	};

private:
	u32											m_current_travel_point;
	bool										m_actual;
	bool										m_failed;
	bool										m_collision;

	xr_vector<STravelPoint>						m_path;
	xr_vector<CLevelGraph::SSegment>			m_segments;
	Fvector										m_start_position;
	Fvector										m_dest_position;
	EDetailPathType								m_path_type;
	xr_map<EMovementParameters,SMovementParams>	m_movement_params;

	friend class CScriptMonster;
	friend class CMotionManager;

protected:
			void	build_path				(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
			void	build_smooth_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
			void	build_dodge_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
			void	build_criteria_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
	IC		void	set_dest_position		(const Fvector &dest_position);
			const	Fvector &direction		();
			bool	actual					() const;
	IC		bool	failed					() const;
	IC		bool	completed				() const;
			bool	valid					() const;
			bool	valid					(const Fvector &position) const;

public:
					CDetailPathManager		();
	virtual			~CDetailPathManager		();
	virtual void	Init					();
};

#include "detail_path_manager_inline.h"