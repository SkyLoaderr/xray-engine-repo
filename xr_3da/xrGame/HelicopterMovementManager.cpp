#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"

#define MAX_ANGULAR_VELOCITY PI
#define MIN_ANGULAR_VELOCITY 0.0f

#define HELI_HUNT_RADIUS 20.0f

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
	Fvector pos, prev_pos, lastDir;

	RCache.OnFrameEnd	();

	float traj_box_size = .505f;
	float path_box_size = .105f;

	for(u32 i=0;i<m_path.size();++i)
	{

		pos = m_path[i].position;
		(i!=0)?prev_pos = m_path[i-1].position:prev_pos=pos;
		
		
//		if(m_path[i].clockwise)
			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
//		else
//			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(255,0,0));
		
/*		if(m_path[i].angularVelocity>0.0001f)
		{
			RCache.dbg_DrawAABB  (pos,path_box_size-0.05f,path_box_size-0.05f,path_box_size-0.05f,D3DCOLOR_XRGB(255,255,0));
		}*/

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
//	m_velocity = 33.0f;//120km4
	m_velocity = 10.0f;//120km4
//	m_curState = eIdleState;
}

void		
CHelicopterMovementManager::deInit()
{
	#ifdef DEBUG
		Device.seqRender.Remove(this);
	#endif
//	m_curState = eIdleState;
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

/*	case CHelicopter::eMovingToAttackTraj:
		{
			Fvector new_pos,w;
			new_pos.set(0.0f,0.0f,0.0f);
			w.set(0.0f,0.0f,0.0f);
			if( getAttackPosition(Level().timeServer(), fTimeDelta,helicopter()->lastEnemyPos(), xform.c, new_pos, w) )
			{
				xform.setXYZ(w);
				xform.c = new_pos;
			}
		
			
		break;
		}
*/
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
	float y_ang_delta = PI_DIV_4*(fTimeDelta);

//	float h_delta	= 1.0f*(fTimeDelta);

/*
  	if( _abs(src.y-pos.y) > h_delta )
	{
		(src.y<pos.y)?pos.y=src.y+h_delta: pos.y=src.y-h_delta;
	}
*/
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
//	m_lastDir.sub(ep, bp);
	
	return true;
}

bool	
CHelicopterMovementManager::getAttackPosition(	u32 time, 
												float fTimeDelta, 
												const Fvector& enemy_pos, 
												const Fvector& src, 
												Fvector& pos, 
												Fvector& dir)
{
//	float

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
		getPathPosition(time,0.0f,helicopter()->XFORM().c,pos,xyz);
		m_keyTrajectory.push_back( SWayPoint(pos, xyz) );
	}
}

void 
CHelicopterMovementManager::setKeyTrajectory(xr_vector<Fvector>& t, bool bFromCurrentPos, bool bClearOld)
{
	m_keyTrajectory.clear();

	if(bFromCurrentPos)
	{
		addCurrentPosToTrajectory();
/*		float h,p,b;
		m_pHelicopter->XFORM().getHPB(h,p,b);
//		m_lastDir.setHP(h,p);
		m_lastXYZ.setHP(h,p);

		m_keyTrajectory.push_back( SWayPoint(m_pHelicopter->XFORM().c), m_lastXYZ );
*/
	}

	m_keyTrajectory.insert(m_keyTrajectory.end(), t.begin(), t.end());
	
/*
	//direction
	Fvector dir;
	trajIt B = m_keyTrajectory.begin();
	trajIt B_ = m_keyTrajectory.begin();
	
	++B;
	for(;B!=m_keyTrajectory.end();++B,++B_)
	{
		dir.sub( (*B).position, (*B_).position ).normalize();
		(*B_).direction = dir;
	}
	//last ???
	m_keyTrajectory.back().direction = (*B_).direction;
*/

/*	if(bGo)
	{
		m_currKeyIdx = 0;
		build_smooth_path(0, true);

		if(!m_failed)
			m_curState = eMovingByPath;
	}
*/
}

void		
CHelicopterMovementManager::shedule_Update(u32 time_delta)
{
	if ( helicopter()->state()==CHelicopter::eInitiateHunt )
	{
		buildHuntPath( helicopter()->lastEnemyPos() );
		if( !failed() )
		{
			helicopter()->setState(CHelicopter::eMovingToAttackTraj);
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

	if( (helicopter()->state()==CHelicopter::eMovingToAttackTraj)&&(m_path.size()) )
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 500 )
		{
			//зайти на атакующую траекторию
			helicopter()->setState(CHelicopter::eInitiateAttackTraj);
		}
	}

	if( (helicopter()->state()==CHelicopter::eMovingByAttackTraj)&&(m_path.size()) )
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 500 )
		{
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
		}
	}

	if( (helicopter()->state()==CHelicopter::eInitiateAttackTraj) )
	{
		m_path.clear();
		build_attack_circle( helicopter()->lastEnemyPos(), helicopter()->XFORM().c, m_path );
		
		pathIt It = m_path.begin();
		Fvector prev_point;
		u32		prev_time;
		for(;It!=m_path.end(); ++It)
		{
			if(It==m_path.begin())
			{
				(*It).time	= Level().timeServer();
				prev_point	= (*It).position;
				prev_time	= (*It).time;
				continue;
			}
			float dist = prev_point.distance_to((*It).position);
			u32 t = prev_time + (dist/m_velocity)*1000;
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
//		setKeyTrajectory(t, true);
		if( m_path.size() )
			addCurrentPosToTrajectory( m_path.back().time-1 );
		else
			addCurrentPosToTrajectory();

		setKeyTrajectory(t, false, false);

		{
			m_currKeyIdx = 0;
			build_smooth_path(0, false, true);
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
