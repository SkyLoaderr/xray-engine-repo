#include "stdafx.h"
#include "custommonster.h"
#include "xr_weapon_list.h"

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
	//*** process look commands
	AI::AIC_Look* L = &q_look;
	switch (L->Command) {
	case AI::AIC_Look::Sleep:
	default:
		// nothing to do....
		break;
	case AI::AIC_Look::Look:
		switch (L->Tmode)
		{
		case AI::t_None:
		default:
			// look to nothing? error!
			L->setLogicError	();
			break;
		case AI::t_Object:
			{
				// look 2 entity
				CEntity*	O = L->Tobject.Object;
				if (0==O)	{
					L->setLogicError();
					break;
				} else {
					// ok, perform look
					Fvector		dir;
					Fvector		pos;
					O->svCenter	(pos);
					dir.sub		(pos,eye_matrix.c);
					mk_rotation	(dir,r_target);
				}
			}
			break;
		case AI::t_Point:
			{
				// look 2 point
				// we cannot verify if point is valid - assume all ok
				Fvector dir;
				//dir.sub(L->Tobject.Point,Position());
				dir.sub(L->Tobject.Point,eye_matrix.c);
				mk_rotation(dir,r_target);
			}
			break;
		case AI::t_Direction:
			{
				// look 2 direction
				Fvector dir = L->Tobject.Direction;
				mk_rotation(dir,r_target);
			}
			break;
		}
		break;
	}
	
//	r_current.yaw = angle_normalize(r_current.yaw);
//	r_target.yaw = angle_normalize(r_target.yaw);
//	r_torso_current.yaw = angle_normalize(r_torso_current.yaw);
//	r_torso_target.yaw = angle_normalize(r_torso_target.yaw);

	angle_lerp_bounds(r_torso_current.yaw,r_torso_target.yaw,r_torso_speed,dt);
	angle_lerp_bounds(r_torso_current.pitch,r_torso_target.pitch,r_torso_speed,dt);

	bool a1 = angle_lerp_bounds(r_current.yaw,r_target.yaw,L->o_look_speed,dt);
	bool a2 = angle_lerp_bounds(r_current.pitch,r_target.pitch,L->o_look_speed,dt);

	if (a1 && a2)
		L->setCompleted();

//	bool a1, a2;
	
//	a1 = angle_lerp	(r_current.yaw,	r_target.yaw,	L->o_look_speed, dt);
//	a2 = angle_lerp	(r_current.pitch,	r_target.pitch,	L->o_look_speed, dt);
//	
//	a1 = angle_lerp	(r_torso_current.yaw,	r_torso_target.yaw,	r_torso_speed, dt);
//	a2 = angle_lerp	(r_torso_current.pitch,	r_torso_target.pitch,	r_torso_speed, dt);

	if (Device.dwTimeGlobal>=L->o_timeout)	L->setTimeout();
	
	mRotate.setHPB					(-NET_Last.o_model,-NET_Last.o_torso.pitch,0);
	
	//
	Engine.Sheduler.Slice			();
}

void CCustomMonster::Exec_Movement	( float dt )
{
	AI_Path.Calculate				(this,vPosition,vPosition,m_fCurSpeed,dt);

	//
	Engine.Sheduler.Slice			();
}
