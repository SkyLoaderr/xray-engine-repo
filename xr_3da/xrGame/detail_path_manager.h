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
			bool	valid					() const;
			bool	valid					(const Fvector &position) const;
			void	build_smooth_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
			void	build_dodge_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
			void	build_criteria_path		(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
protected:
	u32										m_detail_cur_point_index;
	bool									m_detail_path_actual;
public:
	enum EMovementType {
		eMovementTypeStand	= u32(1) << 1,
		eMovementTypJumpUp	= u32(1) << 2,
		eMovementTypJumpDown= u32(1) << 3,
		eMovementTypeWalk	= u32(1) << 4,
		eMovementTypeRun	= u32(1) << 5,
		eMovementTypePanic	= u32(1) << 6,
	};

	enum EDetailPathType {
		eDetailPathTypeSmooth,
		eDetailPathTypeSmoothDodge,
		eDetailPathTypeSmoothCriteria,
	};

	struct SMovementParams {
		float								m_linear_speed;
		float								m_angular_speed;
	};

	struct STravelPoint : public SMovementParams {
		Fvector								m_position;
	};

	bool									m_collision;

	xr_vector<STravelPoint>					m_detail_path;
	xr_vector<CLevelGraph::SSegment>		m_segments;
	Fvector									m_detail_start_position;
	Fvector									m_detail_dest_position;
	EDetailPathType							m_detail_path_type;
	xr_map<EMovementType,SMovementParams>	m_movement_params;

	friend class CScriptMonster;
	friend class CMotionManager;

					CDetailPathManager		();
	virtual			~CDetailPathManager		();
	virtual void	Init					();
			void	build_path				(const xr_vector<u32> &level_path, u32 intermediate_index, const Fvector &dest_position);
			const	Fvector &direction		();
			float	speed					();
			bool	actual					() const;
			bool	completed				() const;
};