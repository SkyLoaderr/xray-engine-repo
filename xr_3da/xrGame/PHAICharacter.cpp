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
bool CPHAICharacter::TryPosition(Fvector pos,bool exact_state){
	if(!b_exist) return false;
	if(m_forced_physics_control||JumpState()) return false;//b_was_on_object||b_on_object||
	if(	DoCollideObj	())	return false;
	Fvector	current_pos;
	GetPosition(current_pos);
	Fvector	cur_vel;GetVelocity(cur_vel);

	Fvector	displace;displace.sub(pos,current_pos);
	float	disp_mag=displace.magnitude();
	
	if(fis_zero(disp_mag)||fis_zero(Device.fTimeDelta))return true;
	const	u32		max_steps=15;
	const	float	fmax_steps=float(max_steps);
	float	fsteps_num=1.f;
	u32		steps_num=1;
	float	disp_pstep=FootRadius();
	if(disp_pstep<disp_mag)
	{
		float	parts	=disp_mag/disp_pstep;
		fsteps_num=ceil(parts);
		steps_num=iCeil(parts);
		if(steps_num>max_steps)
		{
			steps_num	=max_steps	;
			fsteps_num	=fmax_steps	;
			disp_pstep	=disp_mag/fsteps_num;
		}
	}else{
		disp_pstep=disp_mag;
	}

	Fvector	vel;vel.mul(displace,disp_pstep/fixed_step/disp_mag);
	bool	ret=true;
	int save_gm=dBodyGetGravityMode(m_body);
	dBodySetGravityMode(m_body,0);
	for(u32	i=0;steps_num>i;++i)
	{
		SetVelocity(vel);
		Enable();
		if(!step_single(fixed_step))
		{	
			SetVelocity(cur_vel);
			ret=	false;
			break;
		}
	}
	dBodySetGravityMode(m_body,save_gm);
	SetVelocity(cur_vel);
	Fvector	pos_new;GetPosition(pos_new);
	SetPosition(pos_new);
	m_last_move.sub(pos_new,current_pos).mul(1.f/Device.fTimeDelta);
	m_body_interpolation.UpdatePositions();
	m_body_interpolation.UpdatePositions();
	if(ret)
		Disable();
	/*
	dVectorSub(cast_fp(m_last_move),cast_fp(pos),dBodyGetPosition(m_body));
	m_last_move.mul(1.f/Device.fTimeDelta);
	SetPosition(pos);
	m_body_interpolation.UpdatePositions();
	m_body_interpolation.UpdatePositions();
	Disable();
	*/
	return ret;
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
void CPHAICharacter::InitContact(dContact* c,bool	&do_collide,u16 material_idx_1,u16 material_idx_2 )
{
	inherited::InitContact(c,do_collide,material_idx_1,material_idx_2);
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