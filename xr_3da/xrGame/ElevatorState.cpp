#include "stdafx.h"
#include "ElevatorState.h"
#include "ClimableObject.h"
#include "PHCharacter.h"
#include "MathUtils.h"
static const float getting_on_dist		=0.6f;
static const float getting_out_dist		=0.8f;
static const float start_climbing_dist	=0.f;
static const float stop_climbing_dist	=0.1f;
static const float out_dist				=1.5f;
	CElevatorState::CElevatorState()
{
	m_state=clbNone;
	m_ladder=NULL;
	m_character=NULL;
}

void CElevatorState::PhTune(float step)
{
	if(!m_ladder)			return;
	switch(m_state)
	{
	case	clbNone			:UpdateStNone();		break;			
	case 	clbNearUp		:UpdateStNearUp();		break;						
	case 	clbNearDown		:UpdateStNearDown();	break;					
	case 	clbClimbingUp	:UpdateStClimbingUp();	break;					
	case 	clbClimbingDown	:UpdateStClimbingDown();break;	
	case	clbDepart		:UpdateDepart();		break;
	}

}

void CElevatorState::PhDataUpdate(float step)
{

}

void CElevatorState::InitContact(dContact* c,bool &do_collide,SGameMtl * ,SGameMtl * )
{

}

void CElevatorState::SetElevator(CClimableObject* climable)
{
	Fvector d;
	float dist=climable->DDToAxis(m_character,d);
	if(m_ladder==climable||dist>out_dist) return;
	if(m_ladder && m_ladder->DDToAxis(m_character,d)<dist) return;
	SwitchState(clbNone);
	m_ladder=climable;
	
}
void CElevatorState::SetCharacter(CPHCharacter *character)
{
	m_character=character;
	SwitchState(clbNone);
}
void CElevatorState::EvaluateState()
{
	VERIFY(m_ladder&&m_character);
	
	
}

void CElevatorState::SwitchState(Estate new_state)
{
	if((m_state!=clbClimbingUp&&m_state!=clbClimbingDown) &&
	   (new_state==clbClimbingUp||new_state==clbClimbingDown)
 	   )dBodySetGravityMode(m_character->get_body(),0);

	if((new_state!=clbClimbingUp&&new_state!=clbClimbingDown) &&
		(m_state==clbClimbingUp||m_state==clbClimbingDown)
		)dBodySetGravityMode(m_character->get_body(),1);

	if(new_state==clbNone && m_state != clbNone) m_ladder = NULL;
	if(new_state==clbDepart) InitDepart();
	m_state=new_state;
}
void CElevatorState::UpdateStNone()
{

	if(m_ladder->InTouch(m_character))
	{
		Fvector d;m_ladder->DToPlain(m_character,d);
		if(m_character->ControlAccel().dotproduct(d)>0.f)
		{
			SwitchState(clbClimbingUp);
		}
		else
		{
			SwitchState(clbClimbingDown);
		}
	}
	else
	{
		Fvector temp;
		float d_to_lower=m_ladder->DDLowerP(m_character,temp),d_to_upper=m_ladder->DDUpperP(m_character,temp);
		if(d_to_lower<d_to_upper)
		{
			if(getting_on_dist+m_character->FootRadius() > d_to_lower)
															SwitchState(clbNearDown);
		}
		else
		{
			if(getting_on_dist+m_character->FootRadius() > d_to_upper)
															SwitchState(clbNearUp);
		}
	}
}

void CElevatorState::UpdateStNearUp()
{
	Fvector d;
	float dist=m_ladder->DDUpperP(m_character,d);
	if(	m_ladder->InTouch(m_character)				 &&
		d.dotproduct(m_character->ControlAccel())<0.f&&
		m_ladder->DDToPlain(m_character,d)>m_character->FootRadius()/3.f&&
		m_ladder->BeforeLadder(m_character)
		)SwitchState(clbClimbingDown);
	if(dist-m_character->FootRadius()>out_dist)SwitchState((clbNone));
}

void CElevatorState::UpdateStNearDown()
{
	Fvector d;
	float dist=m_ladder->DDLowerP(m_character,d);
	if(	m_ladder->InTouch(m_character)&&
		d.dotproduct(m_character->ControlAccel())>0.f&&
		m_ladder->BeforeLadder(m_character)
		)SwitchState(clbClimbingUp);
	if(dist-m_character->FootRadius()>out_dist)SwitchState((clbNone));
}
void CElevatorState::UpdateStClimbingDown()
{
	//if(m_character->ControlAccel().dotproduct(m_ladder->Axis())>0.f)
	Fvector d;
	m_ladder->DToPlain(m_character,d);
	if(m_character->ControlAccel().dotproduct(d)>0.f)
		SwitchState(clbClimbingUp);
	float to_ax=m_ladder->DDToAxis(m_character,d);
	Fvector ca;ca.set(m_character->ControlAccel());
	float  control_a=to_mag_and_dir(ca);
	if(!fis_zero(to_ax)&&!fis_zero(control_a)&&abs(ca.dotproduct(d))<M_SQRT1_2)SwitchState(clbDepart);
	if(m_ladder->AxDistToLowerP(m_character)-m_character->FootRadius()<stop_climbing_dist)
		SwitchState(clbNearDown);
	
	if(to_ax-m_character->FootRadius()>out_dist)
															SwitchState((clbNone));
	if(fis_zero(control_a)) m_character->ApplyForce(d,m_character->Mass());
}
void CElevatorState::UpdateStClimbingUp()
{
	//if(m_character->ControlAccel().dotproduct(m_ladder->Axis())<0.f)
	Fvector d;
	m_ladder->DToPlain(m_character,d);
	if(m_character->ControlAccel().dotproduct(d)<0.f)
		SwitchState(clbClimbingDown);
	float to_ax=m_ladder->DDToAxis(m_character,d);
	Fvector ca;ca.set(m_character->ControlAccel());
	float control_a=to_mag_and_dir(ca);
	if(!fis_zero(to_ax)&&!fis_zero(control_a)&&abs(ca.dotproduct(d))<M_SQRT1_2)SwitchState(clbDepart);
	if(m_ladder->AxDistToUpperP(m_character)+m_character->FootRadius()<stop_climbing_dist)
		SwitchState(clbNearUp);


	if(to_ax-m_character->FootRadius()>out_dist)
											SwitchState((clbNone));
	if(fis_zero(control_a)) m_character->ApplyForce(d,m_character->Mass());
}

void CElevatorState::GetControlDir(Fvector& dir)
{
	Fvector d;
	switch(m_state)
	{
	case	clbDepart		: 
	case	clbNone			: 		break;			
	case 	clbNearUp		:		 m_ladder->DDUpperP(m_character,d);
									if(m_character->ControlAccel().dotproduct(d)>0.f) dir.set(d);
									break;						
	case 	clbNearDown		:		m_ladder->DDLowerP(m_character,d);
									if(m_character->ControlAccel().dotproduct(d)>0.f) dir.set(d);
									break;					
	case 	clbClimbingUp	:		m_ladder->DDAxis(dir);
									m_ladder->DDToAxis(m_character,d);
									dir.add(d);dir.normalize();
									break;					
	case 	clbClimbingDown	:		m_ladder->DDAxis(dir);
									dir.invert();
									m_ladder->DDToAxis(m_character,d);
									dir.add(d);dir.normalize();
									break;				
	}
}
static const float depart_dist=1.f;
static const u32   depart_time=1000;
void CElevatorState::UpdateDepart()
{
	Fvector temp;
	float d_to_lower=m_ladder->DDLowerP(m_character,temp),d_to_upper=m_ladder->DDUpperP(m_character,temp);
	if(d_to_lower<d_to_upper)
	{
		if(getting_on_dist+m_character->FootRadius() > d_to_lower)
			SwitchState(clbNearDown);
	}
	else
	{
		if(getting_on_dist+m_character->FootRadius() > d_to_upper)
			SwitchState(clbNearUp);
	}

	Fvector p;m_character->GetFootCenter(p);
	p.sub(m_depart_position);
	if(	p.magnitude()>depart_dist || 
		Device.dwTimeGlobal-m_depart_time>depart_time)
		SwitchState(clbNone);

}

void CElevatorState::InitDepart()
{
	m_depart_time=Device.dwTimeGlobal;
	m_character->GetFootCenter(m_depart_position);
}

void CElevatorState::Depart()
{
	if(m_ladder && ClimbingState())SwitchState(clbDepart);
}
void CElevatorState::GetLeaderNormal(Fvector& dir)
{
	if(!m_ladder)	return;
	m_ladder->DDNorm(dir);
	Fvector d;
	m_ladder->DToAxis(m_character,d);
	if(dir.dotproduct(d)>0.f) dir.invert();
}