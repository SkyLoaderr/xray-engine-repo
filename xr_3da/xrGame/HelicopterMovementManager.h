#pragma once

#define NEW_MANAGER

#ifdef NEW_MANAGER
	#undef MOV_MANAGER_OLD
	#define MOV_MANAGER_NEW
#else
#define MOV_MANAGER_OLD
	#undef MOV_MANAGER_NEW
#endif


#include "HelicopterMovementManager_.h"
#include "HelicopterMotion.h"

class CHelicopter;

class CHelicopterMovManager :public CHelicopterMotion
{
	bool							m_bLoop;
	Fmatrix							m_XFORM;

	float							m_baseAltitude;
	float							m_attackAltitude;
	float							m_basePatrolSpeed;
	float							m_baseAttackSpeed;
	float							m_pitch_k;
	Fbox							m_boundingVolume;
	float							m_maxKeyDist;
	float							m_endAttackTime;
	Fvector							m_startDir;
	Fvector							m_stayPoint;
	float							m_time_last_patrol_end;
	float							m_time_last_patrol_start;
	float							m_time_delay_between_patrol;
	float							m_time_patrol_period;
	float							m_time_delay_before_start;
	
	float							m_hunt_dist;
	float							m_hunt_time;
	
	float	_flerp					(float src, float dst, float t)		{return src*(1.f-t) + dst*t;};
	bool	dice					()		{return (::Random.randF(-1.0f, 1.0f) > 0.0f); };

	void	insertRounding			(const Fvector& fromPos, const Fvector& destPos, float radius, xr_vector<Fvector>& vKeys);
	void	makeSmoothKeyPath		(float from_time, const xr_vector<Fvector>& vAddedKeys, xr_vector<Fvector>& vSmoothKeys);
	void	createLevelPatrolTrajectory(u32 keyCount, const Fvector& fromPos, xr_vector<Fvector>& keys );
	void	createHuntPathTrajectory(float from_time, const Fvector& fromPos, const Fvector& enemyPos, xr_vector<Fvector>& keys );
	void	createStayPathTrajectory(const Fvector& fromPos, xr_vector<Fvector>& keys );
	Fvector	makeIntermediateKey		(Fvector& start, Fvector& dest, float k);
	void	fixateKeyPath			(float from_time);
	void	buildHuntPath			(const Fvector& enemyPos);
	void	onFrame					();
	void	onTime					(float t);
	void	insertKeyPoints			(float from_time, xr_vector<Fvector>& keys, float velocity, bool updateHPB=true);
	void	updatePathHPB			(float from_time);
	void	buildHPB				(const Fvector& p_prev, const Fvector& p_prev_phb, const Fvector& p0, const Fvector& p_next, Fvector& p0_phb_res);
	void	addPartolPath			(float from_time);
	void	updatePatrolPath		(float from_time);
	void	addHuntPath				(float from_time, const Fvector& enemyPos);
	void	addHuntPath2			(float from_time, const Fvector& enemyPos);
	void	addPathToStayPoint		(float from_time);
	void	getPathAltitude			(Fvector& point);
	void	truncatePathSafe		(float from_time, float& safe_time, Fvector& lastPoint);

	//patrol path
	void	makeNewPoint			(const Fvector& prevPoint, const Fvector& point, const Fbox& box, Fvector& newPoint);
	void	makeNewPoint			(const Fbox& fbox, const Fvector& point, const Fvector& direction, Fvector& newPoint);
	u16		getPlaneID				(const Fbox& box, const Fvector& point);
	void	getReflectDir			(const Fvector& dir, const u16 planeID, Fvector& newDir);
	void	selectSafeDir			(const Fvector& prevPoint,const Fbox& fbox, Fvector& newDir);
public:
	CHelicopterMovManager			();
	virtual ~CHelicopterMovManager	();
	void	setHuntPathParam		(float dist, float time);

	void	init					(const Fmatrix& heli_xform);
	void	load					(LPCSTR		section);
	void	shedule_Update			(u32 timeDelta, CHelicopter* heli);
	void	getPathPosition			(float time, float fTimeDelta, Fmatrix& dest);

};