#include "stdafx.h"


#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"
#include "GameObject.h"
#include "Car.h"
#include "phaicharacter.h"


bool CPHAICharacter::TryPosition(Fvector pos){
	if(!b_exist) return false;
	if(b_on_object) return false;
	SetPosition(pos);
	m_body_interpolation.UpdatePositions();
	m_body_interpolation.UpdatePositions();
	dBodyDisable(m_body);
	return true;
}

void CPHAICharacter::		SetPosition							(Fvector pos)	
{
	m_vDesiredPosition.set(pos);
	inherited::SetPosition(pos);

}

void CPHAICharacter::BringToDesired(float time,float velocity,float force)
{
	Fvector pos,move;
	GetPosition(pos);

	move.sub(m_vDesiredPosition,pos);
	move.y=0.f;
	float dist=move.magnitude();

	float vel;
	if(dist>EPS_L*100.f)
	{
		vel=dist/time;
		move.mul(1.f/dist);
	}
	else if(dist>EPS_L*10.f)
	{
		vel=dist*dist*dist;
		move.mul(1.f/dist);
	}
	else
	{
		vel=0.f;
		move.set(0,0,0);
	}

	if(vel>velocity)//&&velocity>EPS_L
		vel=velocity;

	if(velocity<EPS_L/fixed_step) 
	{
		vel=0.f;
		move.set(0,0,0);
	}

	SetMaximumVelocity(vel);

	SetAcceleration(move);
}
#ifdef DEBUG
void	CPHAICharacter::OnRender()	
{
	inherited::OnRender();

	if(!b_exist) return;

	Fvector pos;
	GetDesiredPosition(pos);
	pos.y+=m_radius;


	Fvector scale;
	scale.set(0.35f,0.35f,0.35f);
	Fmatrix M;
	M.identity();
	M.scale(scale);
	M.c.set(pos);


	RCache.dbg_DrawEllipse(M, 0xffffffff);
}
#endif