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

void 
CHelicopterMovementManager::OnRender()
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



void		
CHelicopterMovementManager::init(CHelicopter* heli)
{
	m_pHelicopter = heli;
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1);
#endif
}

void		
CHelicopterMovementManager::deInit()
{
	#ifdef DEBUG
		Device.seqRender.Remove(this);
	#endif
}

void		
CHelicopterMovementManager::onFrame(Fmatrix& xform, float fTimeDelta)
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


bool	
CHelicopterMovementManager::getPathPosition(u32 timeCurr, 
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


void	
CHelicopterMovementManager::addCurrentPosToTrajectory(u32 time)
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


void		
CHelicopterMovementManager::shedule_Update(u32 time_delta)
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

		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 )
		{
			build_smooth_path(++m_currKeyIdx, false, false);
			pathIt b;
			STravelPathPoint _p;
			_p.time = lt-2000;
			b = std::lower_bound(m_path.begin(),m_path.end(),_p,time_lesser);
			if(b!=m_path.end())
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
		createLevelPatrolTrajectory(35, t);
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

float	
CHelicopterMovementManager::computeB(float angVel)
{
//	float dv = 1-(angVel-MIN_ANGULAR_VELOCITY);
//	float res = MIN_ANGULAR_VELOCITY*dv + MAX_ANGULAR_VELOCITY*angVel;
	R_ASSERT(angVel>=0.0f);
	return angVel/3.0f;
}

////////////////////////////////////////////////////
/*
void	
CHelicopterMovManager::onTime(float t)
{
	Fvector P,R;
	_Evaluate(t,P,R);
	m_XFORM.setXYZi	(R.x,R.y,R.z);
	m_XFORM.translate_over(P);
}

void 
CHelicopterMovManager::onFrame()
{
	float t = Level().timeServer()/1000.0f;
	
	onTime(t);
}

void	
CHelicopterMovManager::getPathPosition(float time, float fTimeDelta, Fmatrix& dest)
{
	onTime(time);
	dest.set(m_XFORM);
}

bool 
CHelicopterMovManager::dice()
{
	return (::Random.randF(-1.0f, 1.0f) > 0.0f);
}

void	
CHelicopterMovManager::createLevelPatrolTrajectory(u32 keyCount, float fromTime, Fvector fromPos, Fvector fromDir)
{
	xr_vector<Fvector>		keyPoints;
	Fvector					keyPoint;
	Fvector					down_dir;
	bool					useXBound;
	bool					min_max;
	Collide::rq_result		cR;

	down_dir.set(0.0f, -1.0f, 0.0f);

	Fbox levelBox = Level().ObjectSpace.GetBoundingVolume();
	keyPoints.push_back(fromPos);
	for(u32 i = 0; i<keyCount; ++i)	{
		useXBound	= dice();
		min_max		= dice();

		if(useXBound){
			(min_max)?keyPoint.x = levelBox.min.x:keyPoint.x = levelBox.max.x;
			keyPoint.z = ::Random.randF(levelBox.min.z, levelBox.max.z);
		}else{
			(min_max)?keyPoint.z = levelBox.min.z:keyPoint.z = levelBox.max.z;
			keyPoint.x = ::Random.randF(levelBox.min.x, levelBox.max.x);
		}

		keyPoint.y = levelBox.max.y;
		Level().ObjectSpace.RayPick(keyPoint, down_dir, levelBox.max.y-levelBox.min.y+1.0f, Collide::rqtStatic, cR);

		keyPoint.y = keyPoint.y-cR.range+m_baseAltitude;
		//промежуточные точки
		if( keyPoints.size() )
		{
			Fvector& prevPoint = keyPoints.back();
			float dist = prevPoint.distance_to(keyPoint);
			float k = (dist / m_maxKeyDist) - 1.0f;
			for( float i=1; i<k; ++i )
			{
				keyPoints.push_back( makeIntermediateKey(prevPoint, keyPoint, (i/(k+1.0f)) ) );
			}
		}
		keyPoints.push_back(keyPoint);
	};

	float	d;
	float	t;
	Fvector P;
	Fvector R;
	R.set(0.0f,0.0f,1.0f);
	u32 sz = keyPoints.size();

	for(u32 i=0; i<sz; ++i)
	{
		const Fvector& P = keyPoints[i];
		if( i != 0 )
		{
			d  = P.distance_to( keyPoints[i-1] );
			t += m_basePatrolSpeed * d;
		}else
			t = fromTime;

		COMotion::CreateKey(t, P, R);
	}
}

Fvector		
CHelicopterMovManager::makeIntermediateKey(Fvector& start, Fvector& dest, float k)
{
	Fvector point;
	point.lerp(start, dest, k);
	float h = point.y; // or RayPick ????
	point.add( Fvector().random_dir().mul(m_intermediateKeyRandFactor) );
	point.y = h;
	return point;
}

void				
CHelicopterMovManager::buildHPB(const Fvector& p_prev, const Fvector& p0, const Fvector& p_next, Fvector& p0_phb_res)
{
	float s1 = p_prev.distance_to (p0);
	float s2 = p0.distance_to (p_next);
	
	Fvector d1,d2;
	d1.sub(p0, p_prev);
	d2.sub(p_next, p0);

	float p1 = d1.getP();
	float p2 = d2.getP();

	float sk;
	sk = s1+s2/s1;	
	//y
	p0_phb_res.y = lerp(p1, p2, sk);


	//z
	Fvector cp;
	cp.crossproduct (d1,d2);
	p0_phb_res.z = cp.y;

}
*/