#include "stdafx.h"
#include "custommonster.h"

IC void conv_angle(float& c)
{
	if (c<0)				c+=PI_MUL_2;
	else if (c>PI_MUL_2)	c-=PI_MUL_2;
}

void CCustomMonster::mk_rotation	(Fvector &dir, SRotation &R)
{
	// parse yaw
	Fvector DYaw;	
	DYaw.set(dir.x,0.f,dir.z); 
	DYaw.normalize_safe();
	clamp(DYaw.x,-0.9999999f,0.9999999f);
	clamp(DYaw.y,-0.9999999f,0.9999999f);
	clamp(DYaw.z,-0.9999999f,0.9999999f);
	if (DYaw.x>=0)	
		R.yaw = acosf(DYaw.z);
	else			
		R.yaw = 2*PI-acosf(DYaw.z);

	// parse pitch
	dir.normalize_safe	();
	R.pitch				= -asinf(dir.y);
}

void CCustomMonster::Exec_Look		( float dt )
{
	angle_lerp_bounds				(m_body.current.yaw,m_body.target.yaw,m_body.speed,dt);
	angle_lerp_bounds				(m_body.current.pitch,m_body.target.pitch,PI_DIV_6,dt);

//	angle_lerp_bounds				(m_head.current.yaw,m_head.target.yaw,m_head.speed,dt);
//	angle_lerp_bounds				(m_head.current.pitch,m_head.target.pitch,m_head.speed,dt);

	Fvector P						= Position();
	XFORM().setHPB					(-NET_Last.o_model,-NET_Last.o_torso.pitch,0);
	Position()						= P;
}