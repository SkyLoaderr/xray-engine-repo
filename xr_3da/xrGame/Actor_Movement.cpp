// Actor_Movement.cpp:	 ������������ ������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "inventory.h"
#include "weapon.h"
#include "../CameraBase.h"


static const float	s_fLandingTime1		= 0.1f;// ����� ������� ����� ���� Landing1 (�.�. �������� ��������� ��������)
static const float	s_fLandingTime2		= 0.3f;// ����� ������� ����� ���� Landing2 (�.�. �������� ��������� ��������)
static const float	s_fJumpTime			= 0.3f;
static const float	s_fJumpGroundTime	= 0.1f;	// ��� ������ ������ Jump ���� �� �����

	   const float	s_fFallTime			= 0.2f;



void CActor::g_cl_ValidateMState(float dt, u32 mstate_wf)
{
	// ���������� ���������
	if (mstate_wf != mstate_real)
	{
		// ���� �� � ������
		if ((mstate_real&mcCrouch)&&(0==(mstate_wf&mcCrouch)))
		{
			// can we change size to "bbStandBox"
			Fvector				start_pos;
			//bbStandBox.getcenter(start_pos);
			start_pos.add		(Position());
			//if (!g_pGameLevel->ObjectSpace.EllipsoidCollide(CFORM(),XFORM(),start_pos,bbStandBox))
			Fbox stand_box=m_PhysicMovementControl->Boxes()[0];
			//stand_box.y1+=m_PhysicMovementControl->FootExtent().y;
			stand_box.y1+=m_PhysicMovementControl->FootRadius();
			m_PhysicMovementControl->GetPosition(start_pos);
			start_pos.y+=(
				//-(m_PhysicMovementControl->Box().y2-m_PhysicMovementControl->Box().y1)+
				(m_PhysicMovementControl->Boxes()[0].y2-m_PhysicMovementControl->Boxes()[0].y1)
				)/2.f;
			//start_pos.y+=m_PhysicMovementControl->FootExtent().y/2.f;
			start_pos.y+=m_PhysicMovementControl->FootRadius();
			if (!g_pGameLevel->ObjectSpace.EllipsoidCollide(CFORM(),XFORM(),start_pos,stand_box))
			{
				mstate_real &= ~mcCrouch;
				m_PhysicMovementControl->ActivateBox	(0);
			}
		}
	}
	// ��������� �����������
	if (mstate_real&(mcLanding|mcLanding2)){
		m_fLandingTime		-= dt;
		if (m_fLandingTime<=0.f){
			mstate_real		&=~	(mcLanding|mcLanding2);
			mstate_real		&=~	(mcFall|mcJump);
		}
	}
	// ��������� �������
	if (m_PhysicMovementControl->gcontact_Was){
		if (mstate_real&mcFall){
			if (m_PhysicMovementControl->GetContactSpeed()>4.f){
				if (fis_zero(m_PhysicMovementControl->gcontact_HealthLost)){	
					m_fLandingTime	= s_fLandingTime1;
					mstate_real		|= mcLanding;
				}else{
					m_fLandingTime	= s_fLandingTime2;
					mstate_real		|= mcLanding2;
				}
			}
		}
		m_bJumpKeyPressed	=	TRUE;
		m_fJumpTime			=	s_fJumpTime;
		mstate_real			&=~	(mcFall|mcJump);
	}
	if ((mstate_wf&mcJump)==0)	
		m_bJumpKeyPressed	=	FALSE;

	// ������-�� ����/������ - �� ��������
	if (((m_PhysicMovementControl->GetVelocityActual()<0.2f)&&(!(mstate_real&(mcFall|mcJump)))) || m_PhysicMovementControl->bSleep) 
	{
		mstate_real				&=~ mcAnyMove;
	}
	if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)
	{
		// ���� �� ����� �������������� ������� ������ Jump
		if (((s_fJumpTime-m_fJumpTime)>s_fJumpGroundTime)&&(mstate_real&mcJump))
		{
			mstate_real			&=~	mcJump;
			m_fJumpTime			= s_fJumpTime;
		}
	}
	if(m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)

		mstate_real				|=mcClimb;

	else
		mstate_real				&=~mcClimb;

	if(!CanAccelerate()&&isAccelerated(mstate_real))

		mstate_real				^=mcAccel;
}

void CActor::g_cl_CheckControls(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	// ****************************** Check keyboard input and control acceleration
	vControlAccel.set	(0,0,0);

	if (!(mstate_real&mcFall) && (m_PhysicMovementControl->Environment()==CPHMovementControl::peInAir)) 
	{
		m_fFallTime				-=	dt;
		if (m_fFallTime<=0.f){
			m_fFallTime			=	s_fFallTime;
			mstate_real			|=	mcFall;
			mstate_real			&=~	mcJump;
		}
	}

	if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall )
	{
		// jump
		m_fJumpTime				-=	dt;

		if (CanJump() && (mstate_wf&mcJump)/*((mstate_real&mcJump)==0) && (mstate_wf&mcJump) && (m_fJumpTime<=0.f) && !m_bJumpKeyPressed
			&&*/ )
		{
			mstate_real			|=	mcJump;
			m_bJumpKeyPressed	=	TRUE;
			Jump				= m_fJumpSpeed;
			m_fJumpTime			= s_fJumpTime;


			//��������� ���� ������ ��-�� ����������� ������
			if (!psActorFlags.test(AF_GODMODE))	
				ConditionJump((inventory().TotalWeight()+GetMass())/
				(inventory().GetMaxWeight() + GetMass()));
		}

		/*
		if(m_bJumpKeyPressed)
		Jump				= m_fJumpSpeed;
		*/
		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			mstate_real			|=	mcCrouch;
			m_PhysicMovementControl->EnableCharacter();
			m_PhysicMovementControl->ActivateBox(1);
			//m_PhysicMovementControl->ActivateBox(1);
		}

		// mask input into "real" state
		u32 move	= mcAnyMove|mcAccel;
		mstate_real &= (~move);
		mstate_real |= (mstate_wf & move);

		// check player move state
		if (mstate_real&mcAnyMove)
		{
			BOOL	bAccelerated		= isAccelerated(mstate_real)&&CanAccelerate();

			// update player accel
			if (mstate_real&mcFwd)		vControlAccel.z +=  1;
			if (mstate_real&mcBack)		vControlAccel.z += -1;
			if (mstate_real&mcLStrafe)	vControlAccel.x += -1;
			if (mstate_real&mcRStrafe)	vControlAccel.x +=  1;

			// correct "mstate_real" if opposite keys pressed
			if (_abs(vControlAccel.z)<EPS)	mstate_real &= ~(mcFwd+mcBack		);
			if (_abs(vControlAccel.x)<EPS)	mstate_real &= ~(mcLStrafe+mcRStrafe);

			// normalize and analyze crouch and run
			float	scale				= vControlAccel.magnitude();
			if (scale>EPS)	{
				scale	=	m_fWalkAccel/scale;
				if (bAccelerated)			scale *= m_fRunFactor;
				if (mstate_real&mcCrouch)	scale *= m_fCrouchFactor;
				vControlAccel.mul			(scale);
			}else{
				//				mstate_real	&= ~mcAnyMove;
			}
		}
	}else{
		//		mstate_real			&=~ mcAnyMove;
	}

	// transform local dir to world dir
	Fmatrix				mOrient;
	mOrient.rotateY		(-r_model_yaw);
	mOrient.transform_dir(vControlAccel);
}

void CActor::g_Orientate	(u32 mstate_rl, float dt)
{
	// visual effect of "fwd+strafe" like motion
	float calc_yaw = 0;
	switch(mstate_rl&mcAnyMove)
	{
	case mcFwd+mcLStrafe:
	case mcBack+mcRStrafe:
		calc_yaw = +PI_DIV_4; 
		break;
	case mcFwd+mcRStrafe:
	case mcBack+mcLStrafe: 
		calc_yaw = -PI_DIV_4; 
		break;
	case mcLStrafe:
		calc_yaw = +PI_DIV_3-EPS_L; 
		break;
	case mcRStrafe:
		calc_yaw = -PI_DIV_2+EPS_L; 
		break;
	}

	// lerp angle for "effect" and capture torso data from camera
	angle_lerp		(r_model_yaw_delta,calc_yaw,PI_MUL_4,dt);

	// build matrix
	Fmatrix mXFORM;
	mXFORM.rotateY	(-(r_model_yaw + r_model_yaw_delta));
	mXFORM.c.set	(Position());
	XFORM().set		(mXFORM);
}

// ****************************** Update actor orientation according to camera orientation
void CActor::g_cl_Orientate	(u32 mstate_rl, float dt)
{
	// capture camera into torso (only for FirstEye & LookAt cameras)
	if (eacFreeLook!=cam_active){

		r_torso.yaw		=	cam_Active()->GetWorldYaw	();
		r_torso.pitch	=	cam_Active()->GetWorldPitch	();

		unaffected_r_torso_yaw	 = r_torso.yaw;
		unaffected_r_torso_pitch = r_torso.pitch;
	}

	CWeapon *pWeapon = dynamic_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		Fvector dangle;
		dangle = pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso_yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso_pitch + dangle.x;
	}


	// ���� ���� �������� - ��������� ������ �� ������
	if (mstate_rl&mcAnyMove)	{
		r_model_yaw		= angle_normalize(r_torso.yaw);
		mstate_real		&=~mcTurn;
	} else {
		// if camera rotated more than 45 degrees - align model with it
		float ty = angle_normalize(r_torso.yaw);
		if (_abs(r_model_yaw-ty)>PI_DIV_4)	{
			r_model_yaw_dest = ty;
			// 
			mstate_real	|= mcTurn;
		}
		if (_abs(r_model_yaw-r_model_yaw_dest)<EPS_L){
			mstate_real	&=~mcTurn;
		}
		if (mstate_rl&mcTurn){
			angle_lerp	(r_model_yaw,r_model_yaw_dest,PI_MUL_2,dt);
		}
	}
}

void CActor::g_sv_Orientate(u32 /**mstate_rl/**/, float /**dt/**/)
{
	// rotation
	r_model_yaw		= NET_Last.o_model;
	r_torso.yaw		= NET_Last.o_torso.yaw;
	r_torso.pitch	= NET_Last.o_torso.pitch;

	CWeapon *pWeapon = dynamic_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		Fvector dangle;
		dangle = pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso_yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso_pitch + dangle.x;
	}
}


bool	CActor::isAccelerated			(u32 mstate)
{
	if (mstate&mcAccel)
		return psActorFlags.test(AF_ALWAYSRUN)?false:true;
	else
		return psActorFlags.test(AF_ALWAYSRUN)?true :false;
}

bool CActor::CanAccelerate			()
{
	bool can_accel = !m_PhysicMovementControl->PHCapture()&&!(mstate_real&mcBack)&&
		!m_bZoomAimingMode;

	return can_accel;
}

bool	CActor::CanJump				()
{
	bool can_Jump = ((mstate_real&mcJump)==0) && (m_fJumpTime<=0.f) 
		&& !m_bJumpKeyPressed &&!m_bZoomAimingMode;

	return can_Jump;
}