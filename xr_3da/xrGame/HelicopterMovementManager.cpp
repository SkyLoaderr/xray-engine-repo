#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"
#include "level.h"

#define ROUND_RADIUS 20.0f

CHelicopterMovManager::CHelicopterMovManager()
{
}

CHelicopterMovManager::~CHelicopterMovManager	()
{
}

void	CHelicopterMovManager::load(LPCSTR		section)
{
	m_alt_korridor				= 0.1f;
	m_baseAltitude				= 10.0f;
	m_attackAltitude			= 5.0f;
	m_basePatrolSpeed			= 20.0f;
	m_baseAttackSpeed			= 20.0f;
	m_time_last_patrol_end		= 0.0f;
	m_time_last_patrol_start	= 0.0f;
	m_time_delay_between_patrol = 120000.0f;//2min
	m_time_patrol_period		= 120000.0f;//2min
	m_time_delay_before_start	= 1000.0f;	//1sec
	
	m_maxKeyDist				= 35.0f;
	m_startDir.set				(1.0f,0.0f,2.0f);
	m_pitch_k					= 0.006f;

	m_pitch_k					= pSettings->r_float(section,"pitch_koef");
	m_baseAltitude				= pSettings->r_float(section,"altitude");
	m_attackAltitude			= pSettings->r_float(section,"attack_altitude");
	m_basePatrolSpeed			= pSettings->r_float(section,"velocity");
	m_baseAttackSpeed			= pSettings->r_float(section,"attack_velocity");
	m_alt_korridor				= pSettings->r_float(section,"altitude_korridor");

	m_time_delay_between_patrol	= pSettings->r_float(section,"time_delay_between_patrol");
	m_time_patrol_period		= pSettings->r_float(section,"time_patrol_period");
	m_time_delay_before_start	= pSettings->r_float(section,"time_delay_before_start");

	float	x_level_bound				= pSettings->r_float(section,"x_level_bound");
	float	z_level_bound				= pSettings->r_float(section,"z_level_bound");

	m_boundingVolume = Level().ObjectSpace.GetBoundingVolume();
	
	if( 2*x_level_bound<(m_boundingVolume.max.x-m_boundingVolume.max.x)&&
		2*z_level_bound<(m_boundingVolume.max.z-m_boundingVolume.max.z)){

			Fvector b;
			b.set(x_level_bound,0,z_level_bound);
			m_boundingVolume.max.sub(b);
			m_boundingVolume.min.add(b);
		};

	m_pitch_k *= -1.0f;

	m_hunt_dist = 20.0f;
	m_hunt_time = 5.0f;
	
/*	m_korridor					= pSettings->r_float(section,"alt_korridor");
*/

}
void CHelicopterMovManager::init(const Fmatrix& heli_xform)
{
	m_XFORM = heli_xform;
	m_stayPoint = m_XFORM.c;
}

void CHelicopterMovManager::onTime(float t)
{
	Fvector P,R;
	CHelicopterMovManager::_Evaluate(t,P,R);
	m_XFORM.setXYZ	(R.x,R.y,R.z);
	m_XFORM.translate_over(P);
}

void CHelicopterMovManager::onFrame()
{
	float t = Level().timeServer()/1000.0f;
	
	onTime(t);
}

void CHelicopterMovManager::getPathPosition(float time, 
											float fTimeDelta, 
											Fmatrix& dest)
{
	if(m_endTime < time) {
//		Log("----Helicopter: no path (m_endTime<time)");	
		return;
	};
	onFrame ();
	dest.set(m_XFORM);
}


void CHelicopterMovManager::insertKeyPoints(	float from_time, 
												xr_vector<Fvector>& keys,
												float velocity,
												bool updateHPB)
{
	float	t = from_time;
	float	d;
	Fvector R;
	R.set(0.0f,0.0f,0.0f);
	u32 sz = keys.size();

	for(u32 i=0; i<sz; ++i)	{
		const Fvector& P = keys[i];
		if( i != 0 ) {
			d  = P.distance_to( keys[i-1] );
			t += d / velocity;
		}else
			t = from_time;

		CHelicopterMotion::CreateKey(t, P, R);
	};
	
	if(updateHPB)
		updatePathHPB(from_time);

	m_endTime = t;
}

void CHelicopterMovManager::shedule_Update(u32 timeDelta, CHelicopter* heli)
{
	float lt	= Level().timeServer()/1000.0f;;

	if ( heli->state()==CHelicopter::eWaitForStart ) {
		if(lt >= m_time_delay_before_start)
			heli->setState(CHelicopter::eInitiatePatrolZone);
	};

	if (heli->state() == CHelicopter::eInitiatePatrolZone) {
		addPartolPath(lt);
		m_time_last_patrol_start = lt;
		heli->setState(CHelicopter::eMovingByPatrolZonePath);
	};

	if ((heli->state() == CHelicopter::eMovingByPatrolZonePath) ||
		(heli->state() == CHelicopter::eMovingByAttackTraj)		) {
		updatePatrolPath(lt);	
		
		if( (heli->state()==CHelicopter::eMovingByPatrolZonePath) &&
			(lt > m_time_last_patrol_start+m_time_patrol_period) )
				heli->setState(CHelicopter::eInitiateWaitBetweenPatrol);
	};

	if (heli->state() == CHelicopter::eInitiateHunt) {
		addHuntPath(lt, heli->lastEnemyPos() );
		heli->setState(CHelicopter::eMovingByAttackTraj);
		m_endAttackTime = m_endTime;
	};


	if (heli->state() == CHelicopter::eInitiateHunt2) {
		addHuntPath2(lt, heli->lastEnemyPos());
		heli->setState(CHelicopter::eMovingByAttackTraj);
		m_endAttackTime = m_endTime;
	};

	if (heli->state() == CHelicopter::eMovingByAttackTraj) {
		if(lt>m_endAttackTime)
		heli->setState(CHelicopter::eInitiatePatrolZone);
	}
	
	if( (heli->state()==CHelicopter::eInitiateWaitBetweenPatrol) ) {
		addPathToStayPoint(lt);
		heli->setState(CHelicopter::eMovingToWaitPoint);
	};

 	if( heli->state()==CHelicopter::eMovingToWaitPoint) {
		if( (m_endTime - lt) < 1000.0f ) {
			heli->setState(CHelicopter::eWaitBetweenPatrol);
			m_time_last_patrol_end		= lt;
			m_time_last_patrol_start	= 0.0f;
		};

	};

	if( heli->state()==CHelicopter::eWaitBetweenPatrol) {
		if( lt >= m_time_last_patrol_end+m_time_delay_between_patrol )
			heli->setState(CHelicopter::eInitiatePatrolZone);
	};

}

void CHelicopterMovManager::truncatePathSafe(float from_time, 
											 float& safe_time, 
											 Fvector& lastPoint)
{
	safe_time		= 0.0f;

	if(m_endTime > from_time){
		float	minT=0.0f;
		float	maxT=0.0f;
		u32		minIdx=0;
		u32		maxIdx=0;

		u32 sz = CHelicopterMotion::KeyCount();
		VERIFY(sz>4);

		FindNearestKey(from_time, minT, maxT, minIdx, maxIdx);

		if( maxIdx < (sz-2) )// 2 key ahead
			DropHeadKeys(sz-maxIdx-2);

		safe_time = m_endTime;

	}else
		safe_time = from_time;


	Fvector fromRot;

	CHelicopterMotion::_Evaluate(safe_time, lastPoint, fromRot);

}

void CHelicopterMovManager::addPartolPath(float from_time)
{
	float t		= 0.0f;

	(m_endTime > from_time)? t=m_endTime : t=from_time;
	
	Fvector fromPos,fromRot;
	
	if( CHelicopterMotion::KeyCount() > 0 )
		CHelicopterMotion::_Evaluate(t,fromPos,fromRot);
	else
		fromPos = m_XFORM.c;

	xr_vector<Fvector> vAddedKeys;
	createLevelPatrolTrajectory(6, fromPos, vAddedKeys);

	xr_vector<Fvector> vSmoothKeys;
	makeSmoothKeyPath(t, vAddedKeys, vSmoothKeys);
	insertKeyPoints(t, vSmoothKeys, m_basePatrolSpeed,false);
	fixateKeyPath(t);
//	CHelicopterMotion::NormalizeKeys(t,m_endTime,m_basePatrolSpeed);
	updatePathHPB(t);
}

void	CHelicopterMovManager::addHuntPath(float from_time, const Fvector& enemyPos)
{
	Fvector fromPos;

	float safe_time;

	truncatePathSafe(from_time, safe_time, fromPos);

	xr_vector<Fvector> vAddedKeys;

	createHuntPathTrajectory(from_time, fromPos, enemyPos, vAddedKeys);
	insertKeyPoints(safe_time, vAddedKeys, m_baseAttackSpeed, false);
	fixateKeyPath(safe_time);
//	CHelicopterMotion::NormalizeKeys(safe_time,m_endTime,m_baseAttackSpeed);
	updatePathHPB(safe_time);

}

void	CHelicopterMovManager::addHuntPath2(float from_time, const Fvector& enemyPos)
{
	Fvector fromPos;
	Fvector dstPos = enemyPos;
	float safe_time;

	truncatePathSafe(from_time, safe_time, fromPos);

	xr_vector<Fvector> vAddedKeys;
	Fvector dir;
	dir.sub(dstPos, fromPos).normalize_safe();
	float dist = fromPos.distance_to(dstPos);
	if(dist>m_hunt_dist){
		dstPos.mad(fromPos,dir,dist-m_hunt_dist);
	}

	createHuntPathTrajectory(from_time, fromPos, dstPos, vAddedKeys);
	insertKeyPoints(safe_time, vAddedKeys, m_baseAttackSpeed, false);
	fixateKeyPath(safe_time);
//	CHelicopterMotion::NormalizeKeys(safe_time,m_endTime,m_basePatrolSpeed);

	dstPos = vAddedKeys.back();
	
	Fvector T,R;
	CHelicopterMotion::_Evaluate(m_endTime-0.1f,T,R);
	dir.sub(dstPos,T).normalize_safe();
/*	dstPos2.mad(dstPos,dir,1.0f);
	vAddedKeys.clear();
	vAddedKeys.push_back(dstPos);
	vAddedKeys.push_back(dstPos2);*/

	vAddedKeys.clear();
	createRocking(dstPos, dir, vAddedKeys);
	insertKeyPoints(m_endTime, vAddedKeys, 1.0f/m_hunt_time, false);
	updatePathHPB(safe_time);

}

void CHelicopterMovManager::createRocking(const Fvector& fromPos, const Fvector& dir, xr_vector<Fvector>& keys )
{
/*	Fvector p;
	p.mad(fromPos,dir,1.0f);
	keys.push_back(fromPos);
	keys.push_back(p);
*/

	Fvector p;
	Fvector r_dir;
	keys.push_back(fromPos);
	for(float i=0.3f; i<1.0f; i+=0.3f){
		r_dir.random_dir(dir,(PI_DIV_6/2.0f);
		p.mad(fromPos, r_dir, i );
		keys.push_back(p);
	}

}

void CHelicopterMovManager::createHuntPathTrajectory(float from_time, 
													 const Fvector& fromPos, 
													 const Fvector& enemyPos, 
													 xr_vector<Fvector>& keys)
{
	Fvector destPos = enemyPos;
//	destPos.y += m_attackAltitude;
	getPathAltitude(destPos, m_attackAltitude);
	keys.push_back(fromPos);//tmp

//rounding
	Fvector imPoint,T,R;
	CHelicopterMotion::_Evaluate(from_time-1.0f, T, R);

	//calc turn angle
	Fvector dir_prev, dir_next;
	dir_prev.sub(fromPos,T).normalize_safe();
	dir_next.sub(fromPos,destPos).normalize_safe();
	float dp = dir_prev.dotproduct(dir_next);
	float angle = acosf(dp);
	if(angle<PI_DIV_3){//make round
		Fvector cp;
		Fvector norm;
		cp.crossproduct(dir_prev,dir_next);
		dir_prev.mul(-1.0f);
		norm.set(-dir_prev.z,0,dir_prev.x);
		if(cp.y>0)
			norm.mul(-1.0f);

		Fvector tmp;
		tmp.mad(fromPos,norm,15.0f);

		dir_prev.mul(-1.0f);//tmp
		imPoint.mad(tmp,dir_prev,15.0f);
		dir_prev.mul(-1.0f);
		imPoint.y = fromPos.y;
		keys.push_back(imPoint);
		
		imPoint.mad(fromPos,norm,30.0f);
		imPoint.y = fromPos.y;
		keys.push_back(imPoint);
	};
	keys.push_back(destPos);

}


void CHelicopterMovManager::addPathToStayPoint (float from_time)
{
	Fvector fromPos;
	float safe_time;

	truncatePathSafe(from_time, safe_time, fromPos);

	xr_vector<Fvector> vAddedKeys;

	createStayPathTrajectory(fromPos, vAddedKeys);
	insertKeyPoints(safe_time, vAddedKeys, m_basePatrolSpeed,false);
	fixateKeyPath(safe_time);
//	CHelicopterMotion::NormalizeKeys(safe_time,m_endTime,m_basePatrolSpeed);
	updatePathHPB(safe_time);
}

void CHelicopterMovManager::createStayPathTrajectory(const Fvector& fromPos, xr_vector<Fvector>& keys)
{
	keys.push_back(fromPos);
	keys.push_back(m_stayPoint);
}


void CHelicopterMovManager::updatePatrolPath(float t)
{
	u32 minIdx = 0;
	u32 maxIdx = 0;
	float minT = 0.0f;
	float maxT = 0.0f;

	u32 sz = CHelicopterMotion::KeyCount();
	VERIFY(sz>=4);

	CHelicopterMotion::FindNearestKey(t, minT, maxT, minIdx, maxIdx);

	if( maxIdx >= (sz-2) )//2 key ahead
		addPartolPath(t);

	if(minIdx > 6) {
		DropTailKeys(4);
	}
}

void	CHelicopterMovManager::updatePathHPB(float from_time)
{
	u32 minIdx = 0;
	u32 maxIdx = 0;
	float minT = 0.0f;
	float maxT = 0.0f;

	u32 sz = CHelicopterMotion::KeyCount();
	if(!sz)return;

	CHelicopterMotion::FindNearestKey(from_time, minT, maxT, minIdx, maxIdx);

	u32 i;
	//(minIdx == maxIdx)
	Fvector p_prev, p_prev_hpb, p0, p_next, p0_phb_res;
	float p0_time;

	for(i=minIdx; i<sz-2 ;++i) {
		CHelicopterMotion::GetKey(i, p_prev, p_prev_hpb);
		CHelicopterMotion::GetKeyT(i+1, p0);
		CHelicopterMotion::GetKeyT(i+2, p_next);
		CHelicopterMotion::GetKeyTime(i+1, p0_time);

		buildHPB(p_prev, p_prev_hpb, p0, p_next, p0_phb_res);

		CHelicopterMotion::CreateKey(p0_time, p0, p0_phb_res);
	}

}

void CHelicopterMovManager::buildHPB(const Fvector& p_prev, 
									 const Fvector& p_prev_phb,
									 const Fvector& p0, 
									 const Fvector& p_next, 
									 Fvector& p0_phb_res)
{
	float s1 = p_prev.distance_to (p0);
	float s2 = p0.distance_to (p_next);

	Fvector d1,d2;
	d1.sub(p0, p_prev).normalize_safe();
	d2.sub(p_next, p0).normalize_safe();

	float h1 = d1.getH();
	float h2 = d2.getH();
	float p1 = d1.getP();
//	float p2 = d2.getP();

	Fvector cp;

	float sk;
	//y
	sk = s2/(s1+s2);	
//	float newY	= _flerp(h1, h2, sk);
	float newY	= h2;
	float y_sign;
	Fvector prev_d;
	prev_d.setHP(p_prev_phb.y, p_prev_phb.x);

	cp.crossproduct (prev_d,d2);
	(cp.y>0.0)?y_sign=1.0f:y_sign=-1.0f;
	
	p0_phb_res.y = p_prev_phb.y + angle_difference(p_prev_phb.y,newY)*(-y_sign);



	//z
//	Fvector cp;
	float sign;
	cp.crossproduct (d1,d2);
	(cp.y>0.0)?sign=1.0f:sign=-1.0f;

	float ang_diff = angle_difference (h1, h2);
	
	p0_phb_res.z = ang_diff*sign*0.4f;

	//x
	p0_phb_res.x = -p1+m_pitch_k*m_basePatrolSpeed;
	clamp(p0_phb_res.x, -PI_DIV_8, PI_DIV_8);

}

void CHelicopterMovManager::makeSmoothKeyPath(	float time_from,
												const xr_vector<Fvector>& vKeys, 
												xr_vector<Fvector>& vSmoothKeys)
{
	xr_vector<Fvector> vTmpKeys;
//corner keys
	Fvector p;
	for(u32 i=0; i<vKeys.size();++i){
		p = vKeys[i];
		if(i<=vKeys.size()-2){
			if(p.distance_to_xz(vKeys[i+1])< ROUND_RADIUS){
				if(i==0){
					vTmpKeys.push_back(p);
					continue;
				};
				insertRounding(vKeys[i-1], p, ROUND_RADIUS, vTmpKeys);
				++i;
				continue;
			}else
				vTmpKeys.push_back(p);
		}else
			vTmpKeys.push_back(p);
	}


// smooth edges
	xr_vector<Fvector> vTmpKeys2;
	for(u32 i=0; i<vTmpKeys.size();++i){
		p = vTmpKeys[i];
		if(i==0){
			vTmpKeys2.push_back(p);
			continue;
		};
		
		u32 sz = vTmpKeys.size();
		if(i<=sz-2){
			Fvector dir_prev,dir_next;
			dir_prev.sub(p,vTmpKeys[i-1]).normalize_safe();
			dir_next.sub(p,vTmpKeys[i+1]).normalize_safe();
			
			float dp = dir_prev.dotproduct(dir_next);
			float angle = acosf(dp);
			if(angle<PI_DIV_4){//make circle

				insertRounding(vKeys[i-1], p, ROUND_RADIUS, vTmpKeys2);
			}else
				vTmpKeys2.push_back(p);
		}else
			vTmpKeys2.push_back(p);

	};//for

vSmoothKeys = vTmpKeys2;
//make intermediate keys
/*
	insertKeyPoints(time_from, vTmpKeys2, m_basePatrolSpeed, false);
	u32 cnt = CHelicopterMotion::KeyCount();
	u32 idx = cnt - vTmpKeys2.size();

	for(;idx<vTmpKeys2.size();++idx){
		float t;
		CHelicopterMotion::GetKeyTime(idx,t);
		Fvector T,R;
		CHelicopterMotion::_Evaluate(t,T,R);
		getPathAltitude(T);
		vSmoothKeys.push_back(T);
		float end_time;
		if(idx==vTmpKeys2.size()-1)
			end_time=m_endTime;
		else{
			CHelicopterMotion::GetKeyTime(idx+1,end_time);
		}

		for (t+=5.0f;t<(end_time-5.0f);t+=3.0f) {
			CHelicopterMotion::_Evaluate(t,T,R);
			getPathAltitude(T);
			vSmoothKeys.push_back(T);
		}
	}
	CHelicopterMotion::DropHeadKeys(vTmpKeys2.size());
*/
}

void CHelicopterMovManager::fixateKeyPath(float from_time)
{
	xr_vector<Fvector> vAddKeys;
	xr_vector<float>   vAddTime;

	float min_t,max_t;
	u32 min_idx,max_idx;
	Fvector T,R;
	for(float t=from_time; t<m_endTime; t+=3.0f){
		FindNearestKey(t, min_t, max_t, min_idx, max_idx);
		if( (_abs(t-min_t)<5.0f)||(_abs(t-max_t)<5.0f) )
			continue;
		CHelicopterMotion::_Evaluate(t,T,R);
		getPathAltitude(T,m_baseAltitude);
		vAddKeys.push_back(T);
		vAddTime.push_back(t);
	};

	R.set(0.0f,0.0f,0.0f);
	for(u32 i =0; i<vAddKeys.size(); ++i){
		CHelicopterMotion::CreateKey(vAddTime[i], vAddKeys[i], R);
	}
}


void CHelicopterMovManager::insertRounding(const Fvector& fromPos, 
										   const Fvector& destPos, 
										   float radius,
										   xr_vector<Fvector>& vKeys)
{
		Fvector dir,dir_normale;
		Fvector new_point;

		dir.sub(destPos,fromPos).normalize_safe();
		dir_normale.set(-dir.z, 0.0f, dir.x);

		new_point.mad(destPos,dir_normale,radius);
		vKeys.push_back(new_point);

		new_point.mad(destPos,dir,radius);
		vKeys.push_back(new_point);

		dir_normale.mul(-1.0f);
		new_point.mad(destPos,dir_normale,radius);
		vKeys.push_back(new_point);

}


void CHelicopterMovManager::setHuntPathParam (float dist, float time)
{
	m_hunt_dist = dist;
	m_hunt_time = time;
}
