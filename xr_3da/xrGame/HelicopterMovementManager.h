#pragma once

class CHelicopter;

#define MOV_MANAGER_OLD
//#define MOV_MANAGER_NEW

#ifdef MOV_MANAGER_OLD

class CHelicopterMovementManager
								#ifdef DEBUG
									:public pureRender
								#endif
{
public:
	struct STravelParams {
		float				linear_velocity;
		float				angular_velocity; 
		float				real_angular_velocity; 

		STravelParams		(){}
		STravelParams		(float l, float a) : linear_velocity(l), angular_velocity(a), real_angular_velocity(a) {}
		STravelParams		(float l, float a, float ra) : linear_velocity(l), angular_velocity(a), real_angular_velocity(ra) {}
	};

	struct STravelParamsIndex : public STravelParams {
		u32					index;

		STravelParamsIndex	(){}
		STravelParamsIndex	(float l, float a, u32 i) : STravelParams(l,a), index(i) {}
	};

	struct STravelPathPoint {
		Fvector				position;
		Fvector				direction;
		Fvector				xyz;
		u32					velocity;
		float				angularVelocity;
		bool				clockwise;
		u32					time;
	};

	struct STravelPoint {
		Fvector2			position;
		u32					vertex_id;
	};

	struct SPathPoint : public STravelParams, public STravelPoint {
		Fvector2			direction;
	};

	struct SCirclePoint {
		Fvector2			center;
		float				radius;
		Fvector2			point;
		float				angle;
	};

	struct STrajectoryPoint :
		public SPathPoint,
		public SCirclePoint{
	};


	struct SDist {
		u32					index;
		float				time;
		bool operator<		(const SDist &d1) const {return (time < d1.time);}
	};
	struct SWayPoint{
		Fvector				position;
		Fvector				direction;
		SWayPoint			(Fvector& p, Fvector& d):position(p),direction(d){};
		SWayPoint			(Fvector& p):position(p){direction.set(0.0f, 0.0f,0.0f);};
	};

private:
	CHelicopter*					m_pHelicopter;

	xr_vector<STravelPathPoint>		m_path;
	xr_vector<STravelParamsIndex>	m_startParams;
	xr_vector<STravelParamsIndex>	m_destParams;

	xr_vector<STravelPathPoint>		m_tempPath;

	bool							m_failed;
	bool							m_cyclePath;
	bool							m_tryMinTime;

	typedef xr_vector<STravelPathPoint>::iterator pathIt;
	typedef xr_vector<SWayPoint>::iterator trajIt;

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

	Fvector v3d						(const Fvector2 &vector2d) const {return (Fvector().set(vector2d.x,0.f,vector2d.y));	}
	Fvector2 v2d					(const Fvector &vector3d) const	{return (Fvector2().set(vector3d.x,vector3d.z));}

	IC float _lerp					(float src, float dst, float t);
	bool is_negative				(float a){return(!fis_zero(a) && (a < 0.f));	}

protected:
	xr_map<u32,STravelParams>		m_movementParams;
	xr_vector<SWayPoint>			m_keyTrajectory;
	int								m_currKeyIdx;

	Fvector							m_lastXYZ;


	//smooth path
	bool	init_build				(int startKeyIdx, STrajectoryPoint &start, STrajectoryPoint &dest, float& startH, float& destH, u32 &straight_line_index, u32 &straight_line_index_negative);
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
	float	computeB				(float angVel);
	bool	getPathPosition			(u32 time, float fTimeDelta, const Fvector& src, Fvector& pos, Fvector& dir);

	void	addCurrentPosToTrajectory(u32 time = 0);

	CHelicopter*	helicopter		(){return m_pHelicopter;};

public:
	CHelicopterMovementManager		();
	virtual ~CHelicopterMovementManager();
	void		init				(CHelicopter* heli);
	void		deInit				();
	bool		failed				() {return m_failed;};
	void		shedule_Update		(u32 timeDelta);
	void		build_smooth_path	(int startKeyIdx, bool bClearOld, bool bUseDestOrientation);
	void		onFrame				(Fmatrix& xform, float fTimeDelta);

#ifdef DEBUG
	virtual void OnRender			();
#endif

	void		createLevelPatrolTrajectory(u32 keyCount, xr_vector<Fvector>& keyPoints);
	Fvector		makeIntermediateKey	(Fvector& start, Fvector& dest, float k);


	void		buildHuntPath		(const Fvector& enemyPos);
};
#endif

#include "HelicopterMovementManager_inl.h"

#include "HelicopterMotion.h"
class CHelicopterMovManager :public CHelicopterMotion
{
	bool							m_bLoop;
	Fmatrix							m_XFORM;

	float							m_baseAltitude;
	float							m_attackAltitude;
	float							m_basePatrolSpeed;
	float							m_maxKeyDist;
	float							m_endAttackTime;
	Fvector							m_startDir;
	float							m_time_last_patrol_end;
	float							m_time_last_patrol_start;
	float							m_time_delay_between_patrol;
	float							m_time_patrol_period;
	float							m_time_delay_before_start;

	
	float	_flerp					(float src, float dst, float t)		{return src*(1.f-t) + dst*t;};
	bool	dice					()		{return (::Random.randF(-1.0f, 1.0f) > 0.0f); };

	void	createLevelPatrolTrajectory(u32 keyCount, const Fvector& fromPos, xr_vector<Fvector>& keys );
	void	createHuntPathTrajectory(const Fvector& fromPos, const Fvector& enemyPos, xr_vector<Fvector>& keys );

	Fvector	makeIntermediateKey		(Fvector& start, Fvector& dest, float k);
	void	buildHuntPath			(const Fvector& enemyPos);
	void	onFrame					();
	void	onTime					(float t);
	void	insertKeyPoints			(float from_time, xr_vector<Fvector>& keys);
	void	updatePathHPB			(float from_time);
	void	 buildHPB				(const Fvector& p_prev, const Fvector& p_prev_phb, const Fvector& p0, const Fvector& p_next, Fvector& p0_phb_res);
	void	addPartolPath			(float from_time);
	void	updatePatrolPath		(float t);
	void	addHuntPath				(float from_time, const Fvector& enemyPos);
	
	void	getPathAltitude			(Fvector& point);

	//patrol path
	void	makeNewPoint			(const Fvector& prevPoint, const Fvector& point, const Fbox& box, Fvector& newPoint);
	void	makeNewPoint			(const Fbox& fbox, const Fvector& point, const Fvector& direction, Fvector& newPoint);
	u16		getPlaneID				(const Fbox& box, const Fvector& point);
	void	getReflectDir			(const Fvector& dir, const u16 planeID, Fvector& newDir);
	void	selectSafeDir			(const Fvector& prevPoint,const Fbox& fbox, Fvector& newDir);
public:
	CHelicopterMovManager			();
	virtual ~CHelicopterMovManager	();

	void	init					(const Fmatrix& heli_xform);
	void	load					(LPCSTR		section);
	void	shedule_Update			(u32 timeDelta, CHelicopter* heli);
	void	getPathPosition			(float time, float fTimeDelta, Fmatrix& dest);

};