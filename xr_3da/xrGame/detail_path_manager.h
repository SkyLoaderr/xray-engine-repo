////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "level_graph.h"
#include "ai_object_location.h"

class CDetailPathManager :
	virtual public CAI_ObjectLocation
{
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

	struct STravelParams {
		float			linear_velocity;
		float			angular_velocity; 
	};

	struct STravelPoint {
		Fvector2		position;
		u32				vertex_id;
	};

	struct SPathPoint : public STravelParams, public STravelPoint {
		Fvector2		direction;
	};

	struct SCirclePoint {
		Fvector2		center;
		float			radius;
		Fvector2		point;
		float			angle;
	};

	struct STrajectoryPoint :
		public SPathPoint,
		public SCirclePoint
	{
	};

	struct STravelPathPoint {
		Fvector				position;
		EMovementParameters	velocity;
		void set_position(const Fvector &pos)
		{
			position		= pos;
		}
	};

	struct SDist {
		u32		index;
		float	time;

		bool operator<(const SDist &d1) const
		{
			return		(time < d1.time);
		}
	};

private:
	u32											m_current_travel_point;
	bool										m_actuality;
	bool										m_failed;
	bool										m_collision;

	xr_vector<STravelPathPoint>					m_path;
	xr_vector<CLevelGraph::SSegment>			m_segments;
	Fvector										m_start_position;
	Fvector										m_dest_position;

	EDetailPathType								m_path_type;
	xr_map<EMovementParameters,STravelParams>	m_movement_params;
	xr_vector<STravelPathPoint>					m_travel_line;
	xr_vector<STravelPathPoint>					m_temp_path;
	xr_vector<STravelPoint>						m_key_points;

	// old heritage
	xr_vector<Fvector>							m_tpaPoints;
	xr_vector<Fvector>							m_tpaTravelPath;
	xr_vector<u32>								m_tpaPointNodes;
	xr_vector<Fvector>							m_tpaLine;
	xr_vector<u32>								m_tpaNodes;
	//

	IC		void	adjust_point			(const Fvector2			&source,	float								yaw,			float								magnitude,				  Fvector2						&dest) const;
	IC		void	assign_angle			(float					&angle,		const float							start_yaw,		const float							dest_yaw,			const bool							positive,		const bool							start = true) const;
	IC		void	compute_circles			(STrajectoryPoint		&point,			  SCirclePoint					*circles);
			bool	compute_tangent			(const STrajectoryPoint	&start,		const SCirclePoint					&start_circle,	const STrajectoryPoint				&dest,				const SCirclePoint					&dest_circle,		  SCirclePoint					*tangents);
			bool	build_circle_trajectory	(const STrajectoryPoint &position,		  xr_vector<STravelPathPoint>	*path,				  u32							*vertex_id,			const EMovementParameters			velocity);
			bool	build_line_trajectory	(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  u32							vertex_id,				  xr_vector<STravelPathPoint>	*path,			const EMovementParameters			velocity);
			bool	build_trajectory		(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,				const EMovementParameters			velocity1,		const EMovementParameters			velocity2,	const EMovementParameters	velocity3);
			bool	build_trajectory		(	   STrajectoryPoint &start,			  STrajectoryPoint				&dest,			const SCirclePoint					tangents[4][2],		const u32							tangent_count,		  xr_vector<STravelPathPoint>	*path,			  float					&time,		const EMovementParameters velocity1, const EMovementParameters velocity2, const EMovementParameters velocity3);
			bool	compute_trajectory		(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,					  float							&time,			const EMovementParameters			velocity1,	const EMovementParameters	velocity2,	const EMovementParameters velocity3);
			bool	compute_path			(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*m_tpTravelLine);
		
			void	build_smooth_path		(const xr_vector<u32> &level_path, u32 intermediate_index);
			void	build_criteria_path		(const xr_vector<u32> &level_path, u32 intermediate_index);
			void	build_straight_path		(const xr_vector<u32> &level_path, u32 intermediate_index);
			void	build_dodge_path		(const xr_vector<u32> &level_path, u32 intermediate_index);

protected:
			void	build_path				(const xr_vector<u32> &level_path, u32 intermediate_index);
			const	Fvector direction		();
			bool	actual					() const;
	IC		bool	failed					() const;
	IC		bool	completed				(const Fvector &position) const;
			bool	valid					(const Fvector &position) const;
	IC		u32		curr_travel_point_index	() const;
	IC		void	set_start_position		(const Fvector &start_position);
	IC		void	set_dest_position		(const Fvector &dest_position);
	IC		void	set_path_type			(const EDetailPathType path_type);

	IC		const xr_vector<STravelPathPoint>	&path				() const;
	IC		const STravelPathPoint				&curr_travel_point	() const;
	IC		const Fvector						&start_position		() const;
	IC		const Fvector						&dest_position		() const;

	friend class CScriptMonster;
	friend class CMovementManager;
#ifdef DEBUG
	friend class CLevelGraph;
#endif
public:
					CDetailPathManager		();
	virtual			~CDetailPathManager		();
	virtual void	Init					();
			bool	valid					() const;
			
};

#include "detail_path_manager_inline.h"