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

namespace DetailPathManager {
	enum EDetailPathType {
		eDetailPathTypeSmooth,
		eDetailPathTypeSmoothDodge,
		eDetailPathTypeSmoothCriteria,
	};
};

using namespace DetailPathManager;

class CDetailPathManager :
	virtual public CAI_ObjectLocation
{
public:
	struct STravelParams {
		float			linear_velocity;
		float			angular_velocity; 
		float			real_angular_velocity; 

		STravelParams(){}
		STravelParams(float l, float a) : linear_velocity(l), angular_velocity(a), real_angular_velocity(a) {}
		STravelParams(float l, float a, float ra) : linear_velocity(l), angular_velocity(a), real_angular_velocity(ra) {}
	};

	struct STravelParamsIndex : public STravelParams {
		u32				index;
		
		STravelParamsIndex(){}
		STravelParamsIndex(float l, float a, u32 i) : STravelParams(l,a), index(i) {}
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
		u32					vertex_id;
		u32					velocity;

		IC	void set_position	(const Fvector &pos)
		{
			position		= pos;
		}

		IC	void set_vertex_id	(const u32 _vertex_id)
		{
			vertex_id		= _vertex_id;
		}

		IC	Fvector &get_position	()
		{
			return			(position);
		}

		IC	u32		get_vertex_id	()
		{
			return			(vertex_id);
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
protected:
	xr_map<u32,STravelParams>					m_movement_params;
	u32											m_current_travel_point;
private:
	enum EDirectionType {
		eDirectionTypeFP = u32(0),
		eDirectionTypeFN = u32(1),
		eDirectionTypeSP = u32(0),
		eDirectionTypeSN = u32(2),
		eDirectionTypePP = eDirectionTypeFP | eDirectionTypeSP, // both linear velocities are positive
		eDirectionTypeNN = eDirectionTypeFN | eDirectionTypeSN, // both linear velocities are negative
		eDirectionTypePN = eDirectionTypeFP | eDirectionTypeSN, // the first linear velocity is positive, the second one - negative
		eDirectionTypeNP = eDirectionTypeFN | eDirectionTypeSP, // the first linear velocity is negative, the second one - positive
	};


	bool										m_actuality;
	bool										m_failed;
	bool										m_collision;
	EDetailPathType								m_path_type;

	xr_vector<STravelPathPoint>					m_path;
	xr_vector<CLevelGraph::SSegment>			m_segments;
	Fvector										m_start_position;
	Fvector										m_start_direction;
	Fvector										m_dest_position;
	Fvector										m_dest_direction;

	xr_vector<STravelParamsIndex>				m_start_params;
	xr_vector<STravelParamsIndex>				m_dest_params;
	xr_vector<STravelPathPoint>					m_temp_path;
	xr_vector<STravelPoint>						m_key_points;
	u32											m_desirable_mask;
	u32											m_velocity_mask;
	bool										m_try_min_time;
	bool										m_use_dest_orientation;
	bool										m_state_patrol_path;
	u32											m_last_patrol_point;

	u32											m_time_path_built;

	// old heritage
	xr_vector<Fvector>							m_tpaPoints;
	xr_vector<Fvector>							m_tpaTravelPath;
	xr_vector<u32>								m_tpaPointNodes;
	xr_vector<Fvector>							m_tpaLine;
	xr_vector<u32>								m_tpaNodes;
	//

	IC		bool	check_mask				(u32					mask,			  u32							test) const;
	IC		void	adjust_point			(const Fvector2			&source,		  float							yaw,				  float							magnitude,				  Fvector2						&dest) const;
	IC		void	assign_angle			(float					&angle,		const float							start_yaw,		const float							dest_yaw,			const bool							positive,			const EDirectionType				direction_type,				const bool				start = true) const;
	IC		void	compute_circles			(STrajectoryPoint		&point,			  SCirclePoint					*circles);
			bool	compute_tangent			(const STrajectoryPoint	&start,		const SCirclePoint					&start_circle,	const STrajectoryPoint				&dest,				const SCirclePoint					&dest_circle,		      SCirclePoint					*tangents,					const EDirectionType	direction_type);
			bool	build_circle_trajectory	(const STrajectoryPoint &position,		  xr_vector<STravelPathPoint>	*path,				  u32							*vertex_id,			const u32							velocity);
			bool	build_line_trajectory	(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  u32							vertex_id,				  xr_vector<STravelPathPoint>	*path,				const u32							velocity);
			bool	build_trajectory		(const STrajectoryPoint &start,		const STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,				const u32							velocity1,			const u32							velocity2,					const u32				velocity3);
			bool	build_trajectory		(	   STrajectoryPoint &start,			  STrajectoryPoint				&dest,			const SCirclePoint					tangents[4][2],		const u32							tangent_count,		      xr_vector<STravelPathPoint>	*path,							  float				&time,							const u32 velocity1, const u32				velocity2,		const u32 velocity3);
			bool	compute_trajectory		(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*path,					  float							&time,				const u32							velocity1,					const u32				velocity2,						const u32 velocity3, const EDirectionType	direction_type);
			bool	compute_path			(      STrajectoryPoint &start,			  STrajectoryPoint				&dest,				  xr_vector<STravelPathPoint>	*m_tpTravelLine, 	const xr_vector<STravelParamsIndex> &m_start_params,	const xr_vector<STravelParamsIndex> &m_dest_params,				const u32				straight_line_index,			const u32 straight_line_index_negative);
			void	validate_vertex_position(	   STrajectoryPoint &point) const;
			bool	init_build				(const xr_vector<u32>	&level_path,	  u32							intermediate_index,	  STrajectoryPoint				&start,					  STrajectoryPoint				&dest,					  u32							&straight_line_index,			  u32				&straight_line_index_negative);
			bool	fill_key_points			(const xr_vector<u32>	&level_path,	  u32							intermediate_index,   STrajectoryPoint				&start,					  STrajectoryPoint				&dest);
			void	add_patrol_point		();
			void	build_path_via_key_points(STrajectoryPoint		&start,			  STrajectoryPoint				&dest,				  xr_vector<STravelParamsIndex> &finish_params,		const u32							straight_line_index,const u32							straight_line_index_negative);
			void	build_smooth_path		(const xr_vector<u32>	&level_path,	  u32							intermediate_index);
			void	build_criteria_path		(const xr_vector<u32>	&level_path,	  u32							intermediate_index);
			void	build_straight_path		(const xr_vector<u32>	&level_path,	  u32							intermediate_index);
			void	build_dodge_path		(const xr_vector<u32>	&level_path,	  u32							intermediate_index);

protected:
			void	build_path				(const xr_vector<u32> &level_path, u32 intermediate_index);
	IC		void	set_start_position		(const Fvector &start_position);
	IC		void	set_start_direction		(const Fvector &start_direction);
	IC		void	set_dest_position		(const Fvector &dest_position);
	IC		void	set_dest_direction		(const Fvector &dest_direction);
	IC		void	set_path_type			(const EDetailPathType path_type);
	IC		void	set_velocity_mask		(const u32 mask);
	IC		void	set_desirable_mask		(const u32 mask);
	IC		void	set_try_min_time		(const bool try_min_time);
	IC		void	set_use_dest_orientation(const bool use_dest_orientation);
	IC		void	set_state_patrol_path	(const bool state_patrol_path);
	IC		bool	state_patrol_path		() const;

	friend class CScriptMonster;
	friend class CMovementManager;
#ifdef DEBUG
	friend class CLevelGraph;
#endif

public:
					CDetailPathManager		();
	virtual			~CDetailPathManager		();
			void	Init					();
	virtual	void	reinit					();
			bool	valid					() const;
			const	Fvector direction		();
			bool	actual					() const;
	IC		bool	failed					() const;
	IC		bool	completed				(const Fvector &position, bool bRealCompleted = true) const;
			bool	valid					(const Fvector &position) const;
	IC		u32		curr_travel_point_index	() const;

	IC		const xr_vector<STravelPathPoint>	&path					() const;
	IC		const STravelPathPoint				&curr_travel_point		() const;
	IC		const Fvector						&start_position			() const;
	IC		const Fvector						&start_direction		() const;
	IC		const Fvector						&dest_position			() const;
	IC		const Fvector						&dest_direction			() const;
	IC		const u32							velocity_mask			() const;
	IC		const u32							desirable_mask			() const;
	IC		const bool							try_min_time			() const;
	IC		const bool							use_dest_orientation	() const;
	IC		const u32							time_path_built			() const;
};

#include "detail_path_manager_inline.h"