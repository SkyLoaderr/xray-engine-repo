#pragma once

class CHelicopter;



class CHelicopterMovementManager
								#ifdef DEBUG
									:public pureRender
								#endif
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

	struct STravelPathPoint {
		Fvector				position;
		u32					velocity;
		u32					time;
//		Fvector				direction;

		IC	void set_position(const Fvector &pos) {position = pos;}
		IC	Fvector &get_position() {return	(position);	}
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


	struct SDist {
		u32		index;
		float	time;
		bool operator<(const SDist &d1) const {return (time < d1.time);}
	};
	struct SWayPoint{
		Fvector		position;
		Fvector		direction;
		SWayPoint(Fvector& p, Fvector& d):position(p),direction(d){};
		SWayPoint(Fvector& p):position(p){direction.set(0.0f, 0.0f,0.0f);};
	};

	enum EMovementState {
		eIdleState		= u32(0),
		eMovingByPath	= u32(1)
	};
	EMovementState					m_curState;
	u32								m_curPathIdx;
//	u32		m_time_begin;
//	u32		m_time_end;

//	Fvector							m_start_position;
//	Fvector							m_start_direction;
//	Fvector							m_dest_position;
//	Fvector							m_dest_direction;


	xr_vector<STravelPathPoint>		m_path;
	xr_vector<STravelParamsIndex>	m_start_params;
	xr_vector<STravelParamsIndex>	m_dest_params;

	xr_vector<STravelPathPoint>		m_temp_path;

	bool							m_failed;
	bool							m_use_dest_orientation;
	bool							m_cycle_path;
//	u32								m_current_travel_point;
	bool							m_try_min_time;

	typedef xr_vector<STravelPathPoint>::iterator pathIt;
	typedef xr_vector<SWayPoint>::iterator trajIt;
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
	Fvector v3d(const Fvector2 &vector2d) const
	{	return			(Fvector().set(vector2d.x,0.f,vector2d.y));	}

	Fvector2 v2d(const Fvector &vector3d) const
	{return			(Fvector2().set(vector3d.x,vector3d.z));}

protected:
	xr_map<u32,STravelParams>		m_movement_params;
	xr_vector<SWayPoint>			m_keyTrajectory;
	int								m_currKeyIdx;

	CHelicopter*					m_pHelicopter;
	//smooth path
	bool	init_build				(int startKeyIdx, STrajectoryPoint &start, STrajectoryPoint &dest, u32 &straight_line_index, u32 &straight_line_index_negative);
	bool	compute_path			(STrajectoryPoint &start, STrajectoryPoint &dest, xr_vector<STravelPathPoint> *m_tpTravelLine, const xr_vector<STravelParamsIndex> &m_start_params, const xr_vector<STravelParamsIndex> &m_dest_params, const u32 straight_line_index, const u32 straight_line_index_negative);
	void	validate_vertex_position(STrajectoryPoint &point) const;
	bool	compute_trajectory		(STrajectoryPoint &start, STrajectoryPoint &dest, xr_vector<STravelPathPoint> *path, float &time, const u32 velocity1, const u32 velocity2, const u32 velocity3, const EDirectionType direction_type);
	IC		void	compute_circles	(STrajectoryPoint &point, SCirclePoint *circles);
	bool	compute_tangent			(const STrajectoryPoint	&start, const SCirclePoint &start_circle, const STrajectoryPoint &dest, const SCirclePoint &dest_circle, SCirclePoint *tangents, const EDirectionType direction_type);
	bool	build_trajectory		(const STrajectoryPoint &start, const STrajectoryPoint &dest, xr_vector<STravelPathPoint> *path, const u32 velocity1, const u32 velocity2, const u32 velocity3);
	bool	build_trajectory		(STrajectoryPoint &start, STrajectoryPoint &dest, const SCirclePoint tangents[4][2], const u32 tangent_count, xr_vector<STravelPathPoint> *path, float &time, const u32 velocity1, const u32 velocity2, const u32 velocity3);
	IC		void	adjust_point	(const Fvector2 &source, float yaw, float magnitude, Fvector2 &dest) const;
	IC		void	assign_angle	(float &angle, const float start_yaw, const float dest_yaw, const bool positive, const EDirectionType direction_type, const bool start = true) const;
	bool	build_circle_trajectory	(const STrajectoryPoint &position, xr_vector<STravelPathPoint>	*path, const u32 velocity, STravelPathPoint& lastAddedPoint, bool fromCenter);
	bool	build_line_trajectory	(const STravelPathPoint &start, const STrajectoryPoint &dest, xr_vector<STravelPathPoint> *path, const u32 velocity, STravelPathPoint& lastAddedPoint);
	//end smooth path
	
	bool	getPosition(u32 time, Fvector& pos, Fvector& dir);

public:
	CHelicopterMovementManager();
	virtual ~CHelicopterMovementManager();
	void		init(CHelicopter* heli);
	void		deInit();
	void		shedule_Update(u32 time_delta);
	void		build_smooth_path(int startKeyIdx, bool bClearOld);
	void		onFrame(Fmatrix& xform);
#ifdef DEBUG
	virtual void OnRender();
#endif

	void stayIdle();
	void setTrajectory(xr_vector<Fvector>& t, bool bGo=false, bool bFromCurrentPos=false);

};

#include "HelicopterMovementManager_inl.h"