#include "stdafx.h"


#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "../cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"

#include "phaicharacter.h"

#ifdef DEBUG
#	include "../StatGraph.h"
#	include "PHDebug.h"
#	include "level.h"
#	include "debug_renderer.h"
#endif

CPHAICharacter::CPHAICharacter()
{
	m_forced_physics_control=false;
}
void CPHAICharacter::Create(dVector3 sizes)
{
	inherited::Create(sizes);
	m_forced_physics_control=false;//.
}
bool CPHAICharacter::TryPosition(Fvector pos){
	if(!b_exist) return false;
	if(b_was_on_object||b_on_object||m_forced_physics_control||JumpState()) return false;
	dVectorSub(cast_fp(m_last_move),cast_fp(pos),dBodyGetPosition(m_body));
	m_last_move.mul(1.f/Device.fTimeDelta);
	SetPosition(pos);
	m_body_interpolation.UpdatePositions();
	m_body_interpolation.UpdatePositions();
	Disable();
	return true;
}

void CPHAICharacter::		SetPosition							(Fvector pos)	
{
	m_vDesiredPosition.set(pos);
	inherited::SetPosition(pos);

}

void CPHAICharacter::BringToDesired(float time,float velocity,float /**force/**/)
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



void	CPHAICharacter::Jump(const Fvector& jump_velocity)
{
	b_jump=true;
	m_jump_accel.set(jump_velocity);
}
void	CPHAICharacter::	ValidateWalkOn						()
{
//if(b_on_object)
//	ValidateWalkOnObject();
// else 
//	 b_clamb_jump=true;
	inherited::ValidateWalkOn();
}
void CPHAICharacter::InitContact(dContact* c,bool	&do_collide,SGameMtl * material_1,SGameMtl * material_2 )
{
	inherited::InitContact(c,do_collide,material_1,material_2);
	if(is_control||b_lose_control||b_jumping)
												c->surface.mu = 0.00f;
	dxGeomUserData* D1=retrieveGeomUserData(c->geom.g1);
	dxGeomUserData* D2=retrieveGeomUserData(c->geom.g2);
	if(D1&&D2&&D1->ph_object&&D2->ph_object&&D1->ph_object->CastType()==tpCharacter&&D2->ph_object->CastType()==tpCharacter)
	{
		b_on_object=true;
		b_valide_wall_contact=false;
	}
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove))do_collide=false;
#endif
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


	Level().debug_renderer().draw_ellipse(M, 0xffffffff);
}
#endif