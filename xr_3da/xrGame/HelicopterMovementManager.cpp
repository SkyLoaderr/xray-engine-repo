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
	return;
	Fvector pos, prev_pos, lastDir;

	RCache.OnFrameEnd	();

	float traj_box_size = .505f;
	float path_box_size = .105f;

	for(u32 i=0;i<m_path.size();++i)
	{

		pos = m_path[i].position;
		(i!=0)?prev_pos = m_path[i-1].position:prev_pos=pos;
		
		
			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
		

		RCache.dbg_DrawLINE  (Fidentity,prev_pos,pos,D3DCOLOR_XRGB(0,255,0));

	}

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
	case CHelicopter::eMovingToAttackTraj:
	case CHelicopter::eMovingByAttackTraj:
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

	float z_ang_delta = PI_DIV_4*(fTimeDelta);

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
	if ( helicopter()->state()==CHelicopter::eInitiateHunt )
	{
		buildHuntPath( helicopter()->lastEnemyPos() );
		if( !failed() )
		{
			helicopter()->setState(CHelicopter::eMovingByAttackTraj);
		}
	}

	if( (helicopter()->state()==CHelicopter::eMovingByPatrolZonePath)&&(m_path.size()) )
	{
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
		}
	}

	if( (helicopter()->state()==CHelicopter::eMovingByAttackTraj)&&(m_path.size()) )
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 500 )
		{
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
//			helicopter()->setState(CHelicopter::eInitiateAttackTraj);
		}
	}


/*	if( (helicopter()->state()==CHelicopter::eMovingByAttackTraj)&&(m_path.size()) )
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 )
		{
//			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
		}
	}*/

	if( (helicopter()->state()==CHelicopter::eInitiateAttackTraj) )
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
			u32 t = prev_time + (dist/helicopter()->velocity())*1000;
			(*It).time = t;

			prev_point = (*It).position;
			prev_time = t;
		}

		helicopter()->setState(CHelicopter::eMovingByAttackTraj);
	}

	if( (helicopter()->state()==CHelicopter::eInitiatePatrolZone) )
	{
		xr_vector<Fvector> t;
		createLevelPatrolTrajectory(50, t);
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
		}		
	}
}

float	
CHelicopterMovementManager::computeB(float angVel)
{
//	float dv = 1-(angVel-MIN_ANGULAR_VELOCITY);
//	float res = MIN_ANGULAR_VELOCITY*dv + MAX_ANGULAR_VELOCITY*angVel;
	R_ASSERT(angVel>=0.0f);
	return angVel/3.0f;
}
