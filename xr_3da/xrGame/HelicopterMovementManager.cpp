#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"



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


	for(u32 i=0;i<m_path.size();++i)
	{

		pos = m_path[i].position;
		(i!=0)?prev_pos = m_path[i-1].position:prev_pos=pos;
		
		
			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
		

		RCache.dbg_DrawLINE  (Fidentity,prev_pos,pos,D3DCOLOR_XRGB(0,255,0));

	}
*/
/*	for(u32 i=0;i<m_keyTrajectory.size();++i)
	{
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

	switch ( helicopter()->state() )
	{
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
//	case CHelicopter::eMovingToAttackTraj:
	case CHelicopter::eMovingByAttackTraj:
	case CHelicopter::eMovingToWaitPoint:
	case CHelicopter::eInitiateHunt:
		{
			Fvector pos,w;
			pos.set(0.0f,0.0f,0.0f);
			w.set(0.0f,0.0f,0.0f);
			if( getPathPosition(Level().timeServer(),fTimeDelta, xform.c, pos, w) )
			{
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
	if(!m_path.size())
	{
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

	if( !fsimilar(m_lastXYZ.z, xyz.z) )
	{
		if(m_lastXYZ.z < xyz.z)
		{
			m_lastXYZ.z += z_ang_delta;
		}else			
		{
			m_lastXYZ.z -= z_ang_delta;
		}


		xyz.z = m_lastXYZ.z;
	}


	m_lastXYZ = xyz;
	return true;
}


void CHelicopterMovementManager::addCurrentPosToTrajectory(u32 time)
{
	if(time==0)
	{
		float h,p,b;
		helicopter()->XFORM().getHPB(h,p,b);
		m_lastXYZ.setHP(h,p);
		m_keyTrajectory.push_back( SWayPoint(helicopter()->XFORM().c, m_lastXYZ) );
	}else
	{
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
	if ( helicopter()->state()==CHelicopter::eWaitForStart )
	{
		if(Device.dwTimeGlobal >= helicopter()->m_time_delay_before_start)
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};
	
	if ( helicopter()->state()==CHelicopter::eInitiateHunt )
	{
		buildHuntPath( helicopter()->lastEnemyPos() );
		if( !failed() )
		{
			helicopter()->setState(CHelicopter::eMovingByAttackTraj);
		};
	};

	if( (helicopter()->state()==CHelicopter::eMovingByPatrolZonePath)&&(m_path.size()) )
	{
		if(Device.dwTimeGlobal > helicopter()->m_time_last_patrol_start+helicopter()->m_time_patrol_period)
		{
			helicopter()->setState(CHelicopter::eInitiateWaitBetweenPatrol);
			return;
		};

		u32 z = m_keyTrajectory.size();
		if( m_currKeyIdx == (int)z-1 )
		{
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
			return;
		}
			

		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 )
		{
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

	if( (helicopter()->state()==CHelicopter::eMovingByAttackTraj)&&(m_path.size()) )
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 500 )
		{
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
		};
	};

/*	if( (helicopter()->state()==CHelicopter::eInitiateAttackTraj) )
	{
		u32				prev_time;
		Fvector			prev_point;
		u32				sz_old = m_path.size();

		if(sz_old)
		{
			prev_time		= m_path.back().time;
			prev_point		= m_path.back().position;
		}
		else
		{
			prev_time		= Level().timeServer();
			prev_point		= helicopter()->XFORM().c;
		}

		build_attack_circle( helicopter()->lastEnemyPos(), prev_point, m_path );
		
		pathIt It = m_path.begin();
		std::advance(It,sz_old);



		for(;It!=m_path.end(); ++It)
		{

			float dist = prev_point.distance_to((*It).position);
			u32 t = prev_time + (u32)(dist/helicopter()->velocity())*1000;
			(*It).time = t;

			prev_point = (*It).position;
			prev_time = t;
		}

		helicopter()->setState(CHelicopter::eMovingByAttackTraj);
	}*/

	if( (helicopter()->state()==CHelicopter::eInitiateWaitBetweenPatrol) )
	{
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
	if( helicopter()->state()==CHelicopter::eMovingToWaitPoint)
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 )
		{
			helicopter()->setState(CHelicopter::eWaitBetweenPatrol);
			m_path.clear();
			helicopter()->m_time_last_patrol_end		= Device.dwTimeGlobal;
			helicopter()->m_time_last_patrol_start		= 0;
		};
	};

	if( helicopter()->state()==CHelicopter::eWaitBetweenPatrol)
	{
		if( Device.dwTimeGlobal >= helicopter()->m_time_last_patrol_end+helicopter()->m_time_delay_between_patrol)
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};

	if( (helicopter()->state()==CHelicopter::eInitiatePatrolZone) )
	{
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

////////////////////////////////////////////////////

CHelicopterMovManager::CHelicopterMovManager()
{
}

CHelicopterMovManager::~CHelicopterMovManager	()
{
}

void	CHelicopterMovManager::load(LPCSTR		section)
{
	m_baseAltitude				= 20.0f;
	m_basePatrolSpeed			= 20.0f;
	m_maxKeyDist				= 50.0f;
	m_intermediateKeyRandFactor = 3.0f;
}
void CHelicopterMovManager::init(const Fmatrix& heli_xform)
{
	m_XFORM = heli_xform;
}

void CHelicopterMovManager::onTime(float t)
{
	Fvector P,R;
	CHelicopterMovManager::_Evaluate(t,P,R);
	m_XFORM.setXYZi	(R.x,R.y,R.z);
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
	if(m_endTime < time) return;
//	onTime(time);
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

	for(u32 i=0; i<sz; ++i)
	{
		const Fvector& P = keys[i];
		if( i != 0 )
		{
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

	if (heli->state() == CHelicopter::eInitiatePatrolZone)
	{
		addPartolPath(lt);

		heli->setState(CHelicopter::eMovingByPatrolZonePath);
	};

	if (heli->state() == CHelicopter::eMovingByPatrolZonePath)
	{
		updatePatrolPath(lt);	
	};

}

void	CHelicopterMovManager::addPartolPath(float from_time)
{
	float t		= 0.0f;

	(m_endTime > from_time)? t=m_endTime : t=from_time;
	
	Fvector fromPos,fromRot;
	
	if( KeyCount() > 0 )
		_Evaluate(t,fromPos,fromRot);
	else
		fromPos = m_XFORM.c;

	xr_vector<Fvector> vAddedKeys;

	createLevelPatrolTrajectory(10, fromPos, vAddedKeys);
	insertKeyPoints(t, vAddedKeys);

}

void CHelicopterMovManager::updatePatrolPath(float t)
{
	u32 minIdx = 0;
	u32 maxIdx = 0;
	float minT = 0.0f;
	float maxT = 0.0f;

	u32 sz = CHelicopterMotion::KeyCount();
	VERIFY(sz>4);

	CHelicopterMotion::FindNearestKey(t,minT,maxT,minIdx,maxIdx);

	if( maxIdx >= (sz-2) )//2 key ahead
		addPartolPath(t);

	if(minIdx > 6)
	{
		DropTailKeys(4);
		GetKeyTime(0, m_startTime);
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
	Fvector p_prev, p0, p_next, p0_phb_res;
	float p0_time;

	for(i=minIdx; i<sz-2 ;++i)
	{
		CHelicopterMotion::GetKeyT(i, p_prev);
		CHelicopterMotion::GetKeyT(i+1, p0);
		CHelicopterMotion::GetKeyT(i+2, p_next);
		CHelicopterMotion::GetKeyTime(i+1, p0_time);

		buildHPB(p_prev, p0, p_next, p0_phb_res);

		CHelicopterMotion::CreateKey(p0_time, p0, p0_phb_res);
	}

}

void CHelicopterMovManager::buildHPB(const Fvector& p_prev, 
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

	float sk;
	//y
	sk = s2/(s1+s2);	
//	p0_phb_res.y = _flerp(h1, h2, sk);
	
	p0_phb_res.y = h1;
	Log("h1=",h1);
	Log("h2=",h2);

	//z
/*	Fvector cp;
	cp.crossproduct (d1,d2);
	p0_phb_res.z = cp.y;
*/
	p0_phb_res.x = 0.0f;
	p0_phb_res.z = 0.0f;
}
