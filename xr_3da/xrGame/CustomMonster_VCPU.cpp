#include "stdafx.h"
#include "custommonster.h"
#include "..\bodyinstance.h"
#include "xr_weapon_list.h"

IC void conv_angle(float& c)
{
	if (c<0)				c+=PI_MUL_2;
	else if (c>PI_MUL_2)	c-=PI_MUL_2;
}

void CCustomMonster::mk_rotation	(Fvector &dir, SRotation &R)
{
	// parse yaw
	Fvector DYaw;	DYaw.set(dir.x,0.f,dir.z); DYaw.normalize_safe();
	if (DYaw.x>=0)	R.yaw = acosf(DYaw.z);
	else			R.yaw = 2*PI-acosf(DYaw.z);

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
				dir.sub(L->Tobject.Point,Position());
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
	bool a1 = u_lerp_angle(r_current.yaw,	r_target.yaw,	L->o_look_speed, dt);
	bool a2 = u_lerp_angle(r_current.pitch,	r_target.pitch,	L->o_look_speed, dt);
	
	if (a1 && a2) L->setCompleted();

	if (Device.dwTimeGlobal>=L->o_timeout)	L->setTimeout();

	mRotate.setHPB(r_current.yaw,0,0);
}

void CCustomMonster::Exec_Action	( float dt )
{
	//*** process action commands
	AI::C_Command* C	= &q_action;
	AI::AIC_Action* L	= (AI::AIC_Action*)C;
	BOOL	b_firing	= Weapons->isWorking();
	switch (L->Command) {
	case AI::AIC_Action::FireBegin:
		if (!b_firing)	g_cl_fireStart	();
		break;
	case AI::AIC_Action::FireEnd:
		if (b_firing)	g_fireEnd		();
		break;
	default:
		break;
	}
	if (Device.dwTimeGlobal>=L->o_timeout)	L->setTimeout();
}

void CCustomMonster::Exec_Movement	( float dt )
{
	AI_Path.Calculate(this,vPosition,vPosition,5,dt);
}
