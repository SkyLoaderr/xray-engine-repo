#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"
#include "level.h"
#include "../envelope.h"

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
	m_maxKeyDist				= pSettings->r_float(section,"max_key_dist");

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

	m_boundingAssert  = m_boundingVolume;
	m_boundingAssert.scale(0.2f);

	m_hunt_dist = 20.0f;
	m_hunt_time = 5.0f;
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
												bool updateHPB,
												bool normalizeTime)
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
	
	if(normalizeTime){
			// normalize time by real distance
		FloatVec tms;
		tms.push_back			(from_time);

		float min_t,max_t;
		u32 min_idx,max_idx;
		CHelicopterMotion::FindNearestKey(from_time,min_t,max_t,min_idx,max_idx);
		max_idx = CHelicopterMotion::KeyCount();
		max_idx -=1;

		for(u32 i=min_idx; i<max_idx; ++i)	{

			float t_prev,t_next;
			
			CHelicopterMotion::GetKeyTime	(i, t_prev);
			CHelicopterMotion::GetKeyTime	(i+1, t_next);
			
			float dist = CHelicopterMotion::GetDistanceInWay(t_prev, t_next);
			float last_t = tms.back();
			tms.push_back			(last_t+dist/velocity);
		};

		for (int ch=0; ch<ctMaxChannel; ch++){
			CEnvelope* E	= Envelope(EChannelType(ch));
			FloatIt	f_it	= tms.begin();
			
			KeyIt k_it=E->keys.begin();
			std::advance(k_it, min_idx);
			
			for (; f_it!=tms.end(); k_it++,f_it++)
				(*k_it)->time = *f_it;
		}

	};

	if(updateHPB)
		updatePathHPB(from_time);

	CHelicopterMotion::GetKeyTime	(CHelicopterMotion::KeyCount()-1, m_endTime);
}

void CHelicopterMovManager::shedule_Update(u32 timeDelta, CHelicopter* heli)
{
	float lt	= Level().timeServer()/1000.0f;
	

	if ( heli->state()==CHelicopter::eWaitForStart ) {
		if(lt >= m_time_delay_before_start)
			heli->setState(CHelicopter::eInitiatePatrolZone);
	};
	
	if (heli->state() == CHelicopter::eInitiateGoToPoint) {
		addGoToPointPath(lt);
		m_time_last_patrol_start = lt;
//		heli->setState(CHelicopter::eMovingByPatrolZonePath);
		heli->setState(CHelicopter::eMovingToPoint);
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

		int ahead_cnt = sz-maxIdx-1;
		int to_del_cnt = ahead_cnt-3; //оставить спереди 3 ключа

		if( to_del_cnt > 0 )// 2 key ahead
			DropHeadKeys(to_del_cnt);

		safe_time = m_endTime;

	}else
		safe_time = from_time;


	Fvector fromRot;

	if( CHelicopterMotion::KeyCount() )
		CHelicopterMotion::_Evaluate(safe_time, lastPoint, fromRot);
	else
		lastPoint = m_XFORM.c;

}

void CHelicopterMovManager::addPartolPath(float from_time)
{
	float t		= 0.0f;

	(m_endTime > from_time)? t=m_endTime : t=from_time;
	
	Fvector fromPos,fromRot;
	
	int kc = CHelicopterMotion::KeyCount();
	if( kc > 0 )
		CHelicopterMotion::_Evaluate(t,fromPos,fromRot);
	else
		fromPos = m_XFORM.c;

	VERIFY( m_boundingAssert.contains(fromPos) );
	

	xr_vector<Fvector> vAddedKeys;
	createLevelPatrolTrajectory(6, fromPos, vAddedKeys);

	xr_vector<Fvector> vSmoothKeys;
	makeSmoothKeyPath(t, vAddedKeys, vSmoothKeys);
//	insertKeyPoints(t, vSmoothKeys, m_basePatrolSpeed,false, true);
	insertKeyPoints(t, vSmoothKeys, m_basePatrolSpeed,false, false);
	fixateKeyPath(t);
	updatePathHPB(t);
}

void	CHelicopterMovManager::addHuntPath(float from_time, const Fvector& enemyPos)
{
	Fvector fromPos;

	float safe_time;


	truncatePathSafe(from_time, safe_time, fromPos);
	
	xr_vector<Fvector> vAddedKeys;

	createHuntPathTrajectory(from_time, fromPos, enemyPos, vAddedKeys);
//	insertKeyPoints(safe_time, vAddedKeys, m_baseAttackSpeed, false, true);
	insertKeyPoints(safe_time, vAddedKeys, m_baseAttackSpeed, false, false);

	fixateKeyPath(safe_time);

	
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
	insertKeyPoints(safe_time, vAddedKeys, m_baseAttackSpeed, false, false);
	fixateKeyPath(safe_time);

	dstPos = vAddedKeys.back();
	
	Fvector T,R;
	CHelicopterMotion::_Evaluate(m_endTime-0.1f,T,R);
	dir.sub(dstPos,T).normalize_safe();

	vAddedKeys.clear();
	createRocking(dstPos, dir, vAddedKeys, m_hunt_time);
	insertKeyPoints(m_endTime, vAddedKeys, 0.1f, false, false);
	updatePathHPB(safe_time);

}

void CHelicopterMovManager::createRocking(const Fvector& fromPos, const Fvector& dir, xr_vector<Fvector>& keys, float time_ )
{
	Fvector p;
	Fvector r_dir;
	keys.push_back(fromPos);

	float d = 0.0f;
	float r_vel = 0.2f;
	
	for(; d<time_*r_vel; d+=r_vel){
		r_dir.random_dir(dir,(PI_DIV_6/2.0f));
		
		p.mad(fromPos, r_dir, d );
		keys.push_back(p);
	}

/*	for(float i=0.3f; i<1.0f; i+=0.3f){
		r_dir.random_dir(dir,(PI_DIV_6/2.0f));
		p.mad(fromPos, r_dir, i );
		keys.push_back(p);
	}*/

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
		tmp.mad(fromPos,norm,30.0f);

		dir_prev.mul(-1.0f);//tmp
		imPoint.mad(tmp,dir_prev,30.0f);
		dir_prev.mul(-1.0f);
		imPoint.y = fromPos.y;
		keys.push_back(imPoint);
		
		imPoint.mad(fromPos,norm,60.0f);
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
	insertKeyPoints(safe_time, vAddedKeys, m_basePatrolSpeed, false, true);
	fixateKeyPath(safe_time);
//	CHelicopterMotion::NormalizeKeys(safe_time,m_endTime,m_basePatrolSpeed);
	updatePathHPB(safe_time);
}


void CHelicopterMovManager::addGoToPointPath(float from_time)
{
	Fvector fromPos;
	float safe_time;
	
	truncatePathSafe(from_time, safe_time, fromPos);
	
	xr_vector<Fvector> vAddedKeys;

	createHuntPathTrajectory(from_time, fromPos, m_via_point, vAddedKeys);
	getPathAltitude(m_to_point,m_baseAltitude);
	vAddedKeys.push_back(m_to_point);
	insertKeyPoints(safe_time, vAddedKeys, m_basePatrolSpeed, false, false);
	
	
	fixateKeyPath(safe_time);
	

	Fvector dstPos;
	dstPos = vAddedKeys.back();

	Fvector T,R, dir;
	CHelicopterMotion::_Evaluate(m_endTime-0.1f,T,R);
	dir.sub(dstPos,T).normalize_safe();

	vAddedKeys.clear();
	createRocking(dstPos, dir, vAddedKeys, m_wait_in_point);
	insertKeyPoints(m_endTime, vAddedKeys, 0.1f, false, false);

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
//fake	VERIFY(sz>=4);

	CHelicopterMotion::FindNearestKey(t, minT, maxT, minIdx, maxIdx);

	if( maxIdx >= (sz-5) )//5 key ahead
		addPartolPath(t);

	if(minIdx > 6) {
		DropTailKeys(1);
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
	float p0_time, p_prev_time;

	for(i=minIdx; i<sz-2 ;++i) {
		CHelicopterMotion::GetKey(i, p_prev, p_prev_hpb);
		CHelicopterMotion::GetKeyT(i+1, p0);
		CHelicopterMotion::GetKeyT(i+2, p_next);
		CHelicopterMotion::GetKeyTime(i+1, p0_time);
		CHelicopterMotion::GetKeyTime(i, p_prev_time);

		buildHPB(p_prev, p_prev_hpb, p0, p_next, p0_phb_res, p0_time-p_prev_time);

		CHelicopterMotion::CreateKey(p0_time, p0, p0_phb_res);
	};

		// fake for last...
		Fvector T,T2,R,R2;
		float t,t2;
		CHelicopterMotion::GetKey(sz-2,T2,R2);
		CHelicopterMotion::GetKey(sz-1,T,R);
		CHelicopterMotion::GetKeyTime(sz-1,t2);
		CHelicopterMotion::GetKeyTime(sz-1,t);
		CHelicopterMotion::CreateKey(t,T,R2);

}

void CHelicopterMovManager::buildHPB(const Fvector& p_prev, 
									 const Fvector& p_prev_phb,
									 const Fvector& p0, 
									 const Fvector& p_next, 
									 Fvector& p0_phb_res,
									 float time)
{
	if( fis_zero(time) ){
		p0_phb_res = p_prev_phb;
		return;
	}

	float s1 = p_prev.distance_to (p0);
	float s2 = p0.distance_to (p_next);

	Fvector d1,d2;
	d1.sub(p0, p_prev).normalize_safe();
	d2.sub(p_next, p0).normalize_safe();

	float h1 = d1.getH();
	float h2 = d2.getH();
	float p1 = d1.getP();

	Fvector cp;

	float sk;
	//y
	sk = s2/(s1+s2);	
	float newY	= h2;
	float y_sign;
	Fvector prev_d;
	prev_d.setHP(p_prev_phb.y, p_prev_phb.x);

	cp.crossproduct (prev_d,d2);
	(cp.y>0.0)?y_sign=1.0f:y_sign=-1.0f;
	
	p0_phb_res.y = p_prev_phb.y + angle_difference(p_prev_phb.y,newY)*(-y_sign);



	//z
	float sign;
	cp.crossproduct (d1,d2);
	(cp.y>0.0)?sign=1.0f:sign=-1.0f;

	float ang_diff = angle_difference (h1, h2);
	
	p0_phb_res.z = ang_diff*sign*0.4f;

	//x
	float speed_on_section = s1/time;

	p0_phb_res.x = (p1*speed_on_section)/m_basePatrolSpeed + m_pitch_k*speed_on_section;
	clamp(p0_phb_res.x, -PI_DIV_8, PI_DIV_8);
	p0_phb_res.x *= -1.0f;

}

void CHelicopterMovManager::makeSmoothKeyPath(	float time_from,
												const xr_vector<Fvector>& vKeys, 
												xr_vector<Fvector>& vSmoothKeys)
{
//	vSmoothKeys = vKeys;
//	return;
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
				
				vTmpKeys.push_back(p);
				continue;

//				if(i <= vKeys.size()-2 )
//					insertRounding(vKeys[i-1], p, ROUND_RADIUS, vTmpKeys);
//				++i;
//				continue;
			}else
				vTmpKeys.push_back(p);
		}else
			vTmpKeys.push_back(p);
	}

//	vSmoothKeys = vTmpKeys;
//	return;

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
}

void CHelicopterMovManager::fixateKeyPath(float from_time)
{
	xr_vector<Fvector> vAddKeys;
	xr_vector<float>   vAddTime;

	float min_t,max_t;
	u32 min_idx,max_idx;
	Fvector T,R;

	float dTime = (m_baseAttackSpeed>m_basePatrolSpeed)?m_maxKeyDist/m_baseAttackSpeed : m_maxKeyDist/m_basePatrolSpeed;

	for(float t=from_time; t<m_endTime; t+=dTime){
		FindNearestKey(t, min_t, max_t, min_idx, max_idx);
		if( (_abs(t-min_t)<1.5f)||(_abs(t-max_t)<1.5f) )
			continue;
		CHelicopterMotion::_Evaluate(t,T,R);
		getPathAltitude(T,m_baseAltitude);
		vAddKeys.push_back(T);
		VERIFY( m_boundingAssert.contains(T) );

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
		Fvector cent_point;

		dir.sub(destPos,fromPos).normalize_safe();
		dir_normale.set(-dir.z, 0.0f, dir.x);

		cent_point.mad(destPos,dir,radius+3.0f);

		new_point.mad(cent_point,dir_normale,radius);
		vKeys.push_back(new_point);

		new_point.mad(cent_point,dir,radius);
		vKeys.push_back(new_point);

		dir_normale.mul(-1.0f);
		new_point.mad(cent_point,dir_normale,radius);
		vKeys.push_back(new_point);

}

float CHelicopterMovManager::EndTime()
{
	return m_endTime;

}
