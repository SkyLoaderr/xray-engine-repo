#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"

#define MAX_ANGULAR_VELOCITY PI
#define MIN_ANGULAR_VELOCITY 0.0f

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
	Fvector pos, prev_pos;

	RCache.OnFrameEnd	();

	float traj_box_size = .505f;
	float path_box_size = .205f;

	for(u32 i=0;i<m_path.size();++i)
	{

		pos = m_path[i].position;
		(i!=0)?prev_pos = m_path[i-1].position:prev_pos=pos;
		
		if(m_path[i].clockwise)
			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
		else
			RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,0,255));

		RCache.dbg_DrawLINE  (Fidentity,prev_pos,pos,D3DCOLOR_XRGB(0,255,0));

	}
/*
	for(u32 i=0;i<m_keyTrajectory.size();++i)
	{
		Fvector dir;
		Fvector dir_pos;
		dir = m_keyTrajectory[i].direction;
		dir.mul(10.0f);
		dir_pos = m_keyTrajectory[i].position;
		dir_pos.add(dir);

		pos = m_keyTrajectory[i].position;
		RCache.dbg_DrawAABB  (pos,traj_box_size,traj_box_size,traj_box_size,D3DCOLOR_XRGB(255,255,0));
		RCache.dbg_DrawLINE  (Fidentity,pos,dir_pos,D3DCOLOR_XRGB(0,0,255));

	}
*/
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
	m_velocity = 5.0f;//120km4
	m_curState = eIdleState;
}

void		
CHelicopterMovementManager::deInit()
{
	#ifdef DEBUG
		Device.seqRender.Remove(this);
	#endif
	m_curState = eIdleState;
}

void		
CHelicopterMovementManager::onFrame(Fmatrix& xform, float fTimeDelta)
{
	if(Device.dwFrame == 1000)
	{
		float h = 3.0f;
		xr_vector<Fvector> t;
		Fvector pos;
		pos.set(-74.7f, h, -44.4f);
		t.push_back(pos);
		pos.set(-57.5f, h, 5.4f);
		t.push_back(pos);
		pos.set(-25.8f, h, 39.7f);
		t.push_back(pos);
		pos.set(2.4f, h, 27.2f);
		t.push_back(pos);
		pos.set(35.5f, h, -2.5f);
		t.push_back(pos);
		pos.set(26.7f, h, -35.8f);
		t.push_back(pos);
		pos.set(-32.6f, h, -57.4f);
		t.push_back(pos);
//		std::reverse( t.begin(), t.end() );
		setTrajectory(t, true, true);
	}

	switch (m_curState)
	{
	case eIdleState: 
		{
			Fmatrix M, D, R;
			D.set( xform );
			M.rotateY(0.005f);
			R.mul(D, M);
			xform.set( R );
			break;
		}
	case eMovingByPath:
		{
			Fvector pos,w;
			pos.set(0.0f,0.0f,0.0f);
			w.set(0.0f,0.0f,0.0f);
			if( getPosition(Level().timeServer(),fTimeDelta, xform.c, pos, w) )
			{
					xform.setXYZ(w);
					xform.c = pos;
			}
		}
	}//switch
	
}


void 
CHelicopterMovementManager::stayIdle()
{
	m_curState = eIdleState;
}

bool	
CHelicopterMovementManager::getPosition(u32 timeCurr, 
										float fTimeDelta, 
										const Fvector& src, 
										Fvector& pos, 
										Fvector& xyz)
{
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
	
	bool bClockwise = (*b).clockwise;

	if(!fsimilar(m_lastXYZ.z,xyz.z))
	{
		if(m_lastXYZ.z < xyz.z)
			
			if(bClockwise)
				m_lastXYZ.z += PI_DIV_3*(fTimeDelta);
			else
				m_lastXYZ.z -= PI_DIV_3*(fTimeDelta);
		else
			if(bClockwise)
				m_lastXYZ.z -= PI_DIV_3*(fTimeDelta);
			else
				m_lastXYZ.z += PI_DIV_3*(fTimeDelta);

		xyz.z = m_lastXYZ.z;
	}


	m_lastXYZ = xyz;
	m_lastDir.sub(ep, bp);
	
	return true;
}

void 
CHelicopterMovementManager::setTrajectory(xr_vector<Fvector>& t, bool bGo, bool bFromCurrentPos)
{
	m_keyTrajectory.clear();

	if(bFromCurrentPos)
	{
		m_keyTrajectory.push_back( SWayPoint(m_pHelicopter->XFORM().c) );

		float h,p,b;
		m_pHelicopter->XFORM().getHPB(h,p,b);
		m_lastDir.setHP(h,p);
	}

	m_keyTrajectory.insert(m_keyTrajectory.end(), t.begin(), t.end());
	
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


	if(bGo)
	{
		m_currKeyIdx = 0;
		build_smooth_path(0, true);

		if(!m_failed)
			m_curState = eMovingByPath;

	}
}

void		
CHelicopterMovementManager::shedule_Update(u32 time_delta)
{
	if( m_path.size() )
	{
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 )
		{
			build_smooth_path(++m_currKeyIdx, false);
			pathIt b;
			STravelPathPoint _p;
			_p.time = lt-2000;
			b = std::lower_bound(m_path.begin(),m_path.end(),_p,time_lesser);
			if(b!=m_path.end())
				m_path.erase( m_path.begin(), b);
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
