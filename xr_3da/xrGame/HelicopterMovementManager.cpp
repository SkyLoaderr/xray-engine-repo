#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"
#include "level.h"

#ifdef MOV_MANAGER_OLD

bool time_lesser (const CHelicopterMovementManager::STravelPathPoint& el1, 
				  const CHelicopterMovementManager::STravelPathPoint& el2)
{
	return el1.time < el2.time;
}

CHelicopterMovementManager::CHelicopterMovementManager()
{
}

CHelicopterMovementManager::~CHelicopterMovementManager()
{
	deInit();
}
#ifdef DEBUG

void CHelicopterMovementManager::OnRender()
{
/*
float path_box_size = .105f;
Fvector pos=helicopter()->m_left_rocket_bone_xform.c;
RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));

pos=helicopter()->m_right_rocket_bone_xform.c;
RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
*/
/*	return;
	Fvector pos, prev_pos;
	float path_box_size = .105f;

	RCache.OnFrameEnd	();


	for(u32 i=0;i<m_path.size();++i) {

		pos = m_path[i].position;
		(i!=0)?prev_pos = m_path[i-1].position:prev_pos=pos;
		
		
			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
		

		RCache.dbg_DrawLINE  (Fidentity,prev_pos,pos,D3DCOLOR_XRGB(0,255,0));

	}
*/
/*	for(u32 i=0;i<m_keyTrajectory.size();++i) {
		pos = m_keyTrajectory[i].position;
		RCache.dbg_DrawAABB  (pos,traj_box_size,traj_box_size,traj_box_size,D3DCOLOR_XRGB(255,255,0));
	}*/

}
#endif



void CHelicopterMovementManager::init(CHelicopter* heli)
{
	m_pHelicopter = heli;
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1);
#endif
}

void CHelicopterMovementManager::deInit()
{
	#ifdef DEBUG
		Device.seqRender.Remove(this);
	#endif
}

void CHelicopterMovementManager::onFrame(Fmatrix& xform, 
										 float fTimeDelta)
{

	switch ( helicopter()->state() ) {
	case CHelicopter::eIdleState: 
		{
			Fmatrix M, D, R;
			D.set( xform );
			M.rotateY(0.0005f);
			R.mul(D, M);
			xform.set( R );
			break;
		}

	case CHelicopter::eMovingByPatrolZonePath:
	case CHelicopter::eMovingByAttackTraj:
	case CHelicopter::eMovingToWaitPoint:
	case CHelicopter::eInitiateHunt:
		{
			Fvector pos,w;
			pos.set(0.0f,0.0f,0.0f);
			w.set(0.0f,0.0f,0.0f);
			if( getPathPosition(Level().timeServer(),fTimeDelta, xform.c, pos, w) )	{
					xform.setXYZ(w);
					xform.c = pos;
			}
		break;
		}
	}//switch
	
}


bool CHelicopterMovementManager::getPathPosition(	u32 timeCurr, 
													float fTimeDelta, 
													const Fvector& src, 
													Fvector& pos, 
													Fvector& xyz)
{
	if(!m_path.size()){
		pos = src;
		xyz = m_lastXYZ;
		return true;
	}

	pathIt b,e;
	
	STravelPathPoint _p;
	_p.time = timeCurr;

	b = std::lower_bound(m_path.begin(),m_path.end(),_p,time_lesser);
	e = std::upper_bound(m_path.begin(),m_path.end(),_p,time_lesser);
	
	if(b==e)
		--b;

	u32 bt = (*b).time;
	u32 et = (*e).time;
	
	Fvector& bp = (*b).position;
	Fvector& ep = (*e).position;

	Fvector& bxyz = (*b).xyz;
	Fvector& exyz = (*e).xyz;

	float t = (float)(timeCurr-bt)/(float)(et-bt);
	

	pos.lerp(bp,ep,t);

	xyz.x = angle_lerp(bxyz.x, exyz.x, t);
	xyz.y = angle_lerp(bxyz.y, exyz.y, t);
	xyz.z = angle_lerp(bxyz.z, exyz.z, t);
	
	xyz.x = angle_normalize_signed(xyz.x);//-PI..PI
	xyz.y = angle_normalize_signed(xyz.y);
	xyz.z = angle_normalize_signed(xyz.z);

	m_lastXYZ.x = angle_normalize_signed(m_lastXYZ.x);
	m_lastXYZ.y = angle_normalize_signed(m_lastXYZ.y);
	m_lastXYZ.z = angle_normalize_signed(m_lastXYZ.z);

	float z_ang_delta = PI_DIV_4*(fTimeDelta)*HELI_VELOCITY_ROLL_K*helicopter()->velocity();

	if( !fsimilar(m_lastXYZ.z, xyz.z) ) {
		if(m_lastXYZ.z < xyz.z) {
			m_lastXYZ.z += z_ang_delta;
		}else {
			m_lastXYZ.z -= z_ang_delta;
		}

		xyz.z = m_lastXYZ.z;
	}


	m_lastXYZ = xyz;
	return true;
}


void CHelicopterMovementManager::addCurrentPosToTrajectory(u32 time)
{
	if(time==0)	{
		float h,p,b;
		helicopter()->XFORM().getHPB(h,p,b);
		m_lastXYZ.setHP(h,p);
		m_keyTrajectory.push_back( SWayPoint(helicopter()->XFORM().c, m_lastXYZ) );
	}else {
		Fvector xyz;
		Fvector pos;
		Fvector dir;
		getPathPosition(time,0.0f,helicopter()->XFORM().c,pos,xyz);
		dir.setHP(xyz.y, xyz.z);
		m_keyTrajectory.push_back( SWayPoint(pos, dir) );
	}
}


void CHelicopterMovementManager::shedule_Update(u32 time_delta)
{
	if ( helicopter()->state()==CHelicopter::eWaitForStart ) {
		if(Device.dwTimeGlobal >= helicopter()->m_time_delay_before_start)
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};
	
	if ( helicopter()->state()==CHelicopter::eInitiateHunt ) {
		buildHuntPath( helicopter()->lastEnemyPos() );
		if( !failed() ) {
			helicopter()->setState(CHelicopter::eMovingByAttackTraj);
		};
	};

	if( (helicopter()->state()==CHelicopter::eMovingByPatrolZonePath)&&(m_path.size()) ) {
		if(Device.dwTimeGlobal > helicopter()->m_time_last_patrol_start+helicopter()->m_time_patrol_period) {
			helicopter()->setState(CHelicopter::eInitiateWaitBetweenPatrol);
			return;
		};

		u32 z = m_keyTrajectory.size();
		if( m_currKeyIdx == (int)z-1 ) {
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
			return;
		}
			

		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 ) {
			build_smooth_path(++m_currKeyIdx, false, false);
			pathIt b;
			STravelPathPoint _p;
			_p.time = lt-2000;
			b = std::lower_bound(m_path.begin(),m_path.end(),_p,time_lesser);
			
			if( (b!=m_path.end()) && (std::distance(b,m_path.end()) > 5)    )
				m_path.erase( m_path.begin(), b);
		};

		if((u32)m_currKeyIdx >= m_keyTrajectory.size()-1 )
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};

	if( (helicopter()->state()==CHelicopter::eMovingByAttackTraj)&&(m_path.size()) ) {
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 500 ) {
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
		};
	};


	if( (helicopter()->state()==CHelicopter::eInitiateWaitBetweenPatrol) ) {
		m_keyTrajectory.clear();

		if( m_path.size() )
			addCurrentPosToTrajectory( m_path.back().time-1 );
		else
			addCurrentPosToTrajectory();

		m_keyTrajectory.push_back( helicopter()->m_stayPos );

		{
			m_currKeyIdx = 0;
			build_smooth_path(0, false, false);
			if(!m_failed)
				helicopter()->setState(CHelicopter::eMovingToWaitPoint);
		};	
		
	};
	if( helicopter()->state()==CHelicopter::eMovingToWaitPoint) {
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 ) {
			helicopter()->setState(CHelicopter::eWaitBetweenPatrol);
			m_path.clear();
			helicopter()->m_time_last_patrol_end		= Device.dwTimeGlobal;
			helicopter()->m_time_last_patrol_start		= 0;
		};
	};

	if( helicopter()->state()==CHelicopter::eWaitBetweenPatrol) {
		if( Device.dwTimeGlobal >= helicopter()->m_time_last_patrol_end+helicopter()->m_time_delay_between_patrol)
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};

	if( (helicopter()->state()==CHelicopter::eInitiatePatrolZone) ) {
		xr_vector<Fvector> t;
		createLevelPatrolTrajectory(200, t);
		m_keyTrajectory.clear();

		if( m_path.size() )
			addCurrentPosToTrajectory( m_path.back().time-1 );
		else
			addCurrentPosToTrajectory();

		m_keyTrajectory.insert(m_keyTrajectory.end(), t.begin(), t.end());

		{
			m_currKeyIdx = 0;
			build_smooth_path(0, false, false);
			if(!m_failed)
				helicopter()->setState(CHelicopter::eMovingByPatrolZonePath);
		};	
		helicopter()->m_time_last_patrol_start = Device.dwTimeGlobal;
		helicopter()->m_time_last_patrol_end = 0;

	};
	
}

float CHelicopterMovementManager::computeB(float angVel)
{
//	float dv = 1-(angVel-MIN_ANGULAR_VELOCITY);
//	float res = MIN_ANGULAR_VELOCITY*dv + MAX_ANGULAR_VELOCITY*angVel;
	R_ASSERT(angVel>=0.0f);
	return angVel/3.0f;
}
#endif

////////////////////////////////////////////////////

CHelicopterMovManager::CHelicopterMovManager()
{
}

CHelicopterMovManager::~CHelicopterMovManager	()
{
}

void	CHelicopterMovManager::load(LPCSTR		section)
{
	m_baseAltitude				= 10.0f;
	m_attackAltitude			= 5.0f;
	m_basePatrolSpeed			= 20.0f;

	m_time_last_patrol_end		= 0.0f;
	m_time_last_patrol_start	= 0.0f;
	m_time_delay_between_patrol = 120000.0f;//2min
	m_time_patrol_period		= 120000.0f;//2min
	m_time_delay_before_start	= 1000.0f;	//1sec
	
	m_maxKeyDist				= 35.0f;
	m_startDir.set				(1.0f,0.0f,1.5f);


	m_baseAltitude				= pSettings->r_float(section,"altitude");
	m_attackAltitude			= pSettings->r_float(section,"attack_altitude");
	m_basePatrolSpeed			= pSettings->r_float(section,"velocity");

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
												xr_vector<Fvector>& keys)
{
// 1-зафиксировать текущую позицию, чтобы не повлияло на ближайший кусок пути
// 2-удалить ключи > from_time

	float	t = from_time;
	float	d;
	Fvector R;
	R.set(0.0f,0.0f,0.0f);
	u32 sz = keys.size();

	for(u32 i=0; i<sz; ++i)	{
		const Fvector& P = keys[i];
		if( i != 0 ) {
			d  = P.distance_to( keys[i-1] );
			t += d / m_basePatrolSpeed;
		}else
			t = from_time;

		CHelicopterMotion::CreateKey(t, P, R);
	};
	
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

	if (heli->state() == CHelicopter::eMovingByAttackTraj) {
		if(lt>m_endAttackTime)
		heli->setState(CHelicopter::eMovingByPatrolZonePath);
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
	insertKeyPoints(t, vAddedKeys);

}

void	CHelicopterMovManager::addHuntPath(float from_time, const Fvector& enemyPos)
{
/*	float t		= 0.0f;

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

		t = m_endTime;

	}else
		t=from_time;


	Fvector fromPos, fromRot;

	CHelicopterMotion::_Evaluate(t,fromPos,fromRot);
*/
	Fvector fromPos;
	float safe_time;

	truncatePathSafe(from_time, safe_time, fromPos);

	xr_vector<Fvector> vAddedKeys;

	createHuntPathTrajectory(fromPos, enemyPos, vAddedKeys);
	insertKeyPoints(safe_time, vAddedKeys);
}

void CHelicopterMovManager::createHuntPathTrajectory(const Fvector& fromPos, const Fvector& enemyPos, xr_vector<Fvector>& keys)
{
	Fvector destPos = enemyPos;
	destPos.y += m_attackAltitude;
	keys.push_back(fromPos);
	keys.push_back(destPos);
}


void CHelicopterMovManager::addPathToStayPoint (float from_time)
{
	Fvector fromPos;
	float safe_time;

	truncatePathSafe(from_time, safe_time, fromPos);

	xr_vector<Fvector> vAddedKeys;

	createStayPathTrajectory(fromPos, vAddedKeys);
	insertKeyPoints(safe_time, vAddedKeys);
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
	VERIFY(sz>4);

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
	p0_phb_res.x = -p1;
	clamp(p0_phb_res.x, -PI_DIV_8, PI_DIV_8);
	
}

