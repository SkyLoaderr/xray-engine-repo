// Actor_Movement.cpp:	 передвижения актера
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "inventory.h"
#include "weapon.h"
#include "../CameraBase.h"
#include "xrMessages.h"

#include "level.h"
#include "HUDManager.h"
#include "UI.h"
#include "ui/UIMainIngameWnd.h"
#include "string_table.h"



static const float	s_fLandingTime1		= 0.1f;// через сколько снять флаг Landing1 (т.е. включить следующую анимацию)
static const float	s_fLandingTime2		= 0.3f;// через сколько снять флаг Landing2 (т.е. включить следующую анимацию)
static const float	s_fJumpTime			= 0.3f;
static const float	s_fJumpGroundTime	= 0.1f;	// для снятия флажка Jump если на земле
	   const float	s_fFallTime			= 0.2f;

IC static void generate_orthonormal_basis1(const Fvector& dir,Fvector& updir, Fvector& right)
{

	right.crossproduct(dir,updir); //. <->
	right.normalize();
	updir.crossproduct(right,dir);
}


void CActor::g_cl_ValidateMState(float dt, u32 mstate_wf)
{
	// изменилось состояние
	if (mstate_wf != mstate_real)
	{
		
		if ((mstate_real&mcCrouch)&&(0==(mstate_wf&mcCrouch)))
		{

			if (ActivateBox(0))
			{
				mstate_real &= ~mcCrouch;
			}
		}
	}
	// закончить приземление
	if (mstate_real&(mcLanding|mcLanding2)){
		m_fLandingTime		-= dt;
		if (m_fLandingTime<=0.f){
			mstate_real		&=~	(mcLanding|mcLanding2);
			mstate_real		&=~	(mcFall|mcJump);
		}
	}
	// закончить падение
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

	// Зажало-ли меня/уперся - не двигаюсь
	if (((m_PhysicMovementControl->GetVelocityActual()<0.2f)&&(!(mstate_real&(mcFall|mcJump)))) || m_PhysicMovementControl->bSleep) 
	{
		mstate_real				&=~ mcAnyMove;
	}
	if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)
	{
		// если на земле гарантированно снимать флажок Jump
		if (((s_fJumpTime-m_fJumpTime)>s_fJumpGroundTime)&&(mstate_real&mcJump))
		{
			mstate_real			&=~	mcJump;
			m_fJumpTime			= s_fJumpTime;
		}
	}
	if(m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)
	{
		mstate_real				|=mcClimb;
	}
	else
	{
		if (mstate_real & mcClimb)
		{
			RestoreHidedWeapon();
		}
		mstate_real				&=~mcClimb;		
	};

	if(!CanAccelerate()&&isAccelerated(mstate_real))
	{
		mstate_real				^=mcAccel;
	};		
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

	if(!CanMove()) 
	{
		return;
	}

	// update player accel
	if (mstate_wf&mcFwd)		vControlAccel.z +=  1;
	if (mstate_wf&mcBack)		vControlAccel.z += -1;
	if (mstate_wf&mcLStrafe)	vControlAccel.x += -1;
	if (mstate_wf&mcRStrafe)	vControlAccel.x +=  1;

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


			//уменьшить силу игрока из-за выполненого прыжка
			if (!psActorFlags.test(AF_GODMODE))	
				ConditionJump(inventory().TotalWeight()/
				inventory().GetMaxWeight());
		}

		/*
		if(m_bJumpKeyPressed)
		Jump				= m_fJumpSpeed;
		*/
		// crouch
		if ((0==(mstate_real&mcCrouch))&&(mstate_wf&mcCrouch))
		{
			
			m_PhysicMovementControl->EnableCharacter();
			if(ActivateBox(1))mstate_real			|=	mcCrouch;

		}

		// mask input into "real" state
		u32 move	= mcAnyMove|mcAccel;

		if (((mstate_real&mcCrouch)))
		{
			if(0==(mstate_real&mcAccel)&&(mstate_wf&mcAccel))
			{
				m_PhysicMovementControl->EnableCharacter();
				if(!ActivateBox(2))move	&=~mcAccel;
			}

			if((mstate_real&mcAccel)&&0==(mstate_wf&mcAccel))
			{
				m_PhysicMovementControl->EnableCharacter();
				if(ActivateBox(1))mstate_real	&=~mcAccel;
			}

		}



		mstate_real &= (~move);
		mstate_real |= (mstate_wf & move);

	
		
		// check player move state
		if (mstate_real&mcAnyMove)
		{
			BOOL	bAccelerated		= isAccelerated(mstate_real)&&CanAccelerate();



			// correct "mstate_real" if opposite keys pressed
			if (_abs(vControlAccel.z)<EPS)	mstate_real &= ~(mcFwd+mcBack		);
			if (_abs(vControlAccel.x)<EPS)	mstate_real &= ~(mcLStrafe+mcRStrafe);

			// normalize and analyze crouch and run
			float	scale				= vControlAccel.magnitude();
			if (scale>EPS)	{
				scale	=	m_fWalkAccel/scale;
				if (bAccelerated)
					if (mstate_real&mcBack)
						scale *= m_fRunBackFactor;
					else
						scale *= m_fRunFactor;
				else
					if (mstate_real&mcBack)
						scale *= m_fWalkBackFactor;

				if (mstate_real&mcCrouch)	scale *= m_fCrouchFactor;
				if (mstate_real&mcClimb)	scale *= m_fClimbFactor;
				vControlAccel.mul			(scale);
			}else{
				//				mstate_real	&= ~mcAnyMove;
			}
		}
	}else{
		//		mstate_real			&=~ mcAnyMove;
	}

	//transform local dir to world dir
	Fmatrix				mOrient;
	mOrient.rotateY		(-r_model_yaw);
	mOrient.transform_dir(vControlAccel);
	//XFORM().transform_dir(vControlAccel);
	if (mstate_real & mcClimb)
	{
		if (mstate_real&mcAnyMove)
		{
			HideCurrentWeapon();
		}
		else
		{
			RestoreHidedWeapon();
		};
	};
	/*
	if(mstate_real&mcClimb&&mstate_real&mcAnyMove&&
	inventory().ActiveItem()&&inventory().ActiveItem()->HandDependence()==hd2Hand)
	{
		//inventory().ActiveItem()->Deactivate();
		inventory().Activate(NO_ACTIVE_SLOT);
	}
*/
}

void CActor::g_Orientate	(u32 mstate_rl, float dt)
{
	// visual effect of "fwd+strafe" like motion
	float calc_yaw = 0;
	if(mstate_real&mcClimb)
	{
		if(g_LeaderOrient(mstate_rl,dt)) return;
	}
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
bool CActor::g_LeaderOrient(u32/* mstate_rl*/,float dt)
{
	Fvector leader_norm;
	leader_norm.set(m_PhysicMovementControl->GroundNormal());
	if(_abs(leader_norm.y)>M_SQRT1_2) return false;
	//leader_norm.y=0.f;
	float mag=leader_norm.magnitude();
	if(mag<EPS_L) return false;
	leader_norm.div(mag);
	leader_norm.invert();
	Fmatrix M;
	M.k.set(leader_norm);
	M.j.set(0.f,1.f,0.f);
	generate_orthonormal_basis1(M.k,M.j,M.i);
	M.i.invert();
	//M.j.invert();


	Fquaternion q1,q2,q3;
	q1.set(XFORM());
	q2.set(M);
	q3.slerp(q1,q2,dt);
	//Fvector angles1,angles2,angles3;
	//XFORM().getHPB(angles1.x,angles1.y,angles1.z);
	//M.getHPB(angles2.x,angles2.y,angles2.z);
	////angle_lerp(angles3.x,angles1.x,angles2.x,dt);
	////angle_lerp(angles3.y,angles1.y,angles2.y,dt);
	////angle_lerp(angles3.z,angles1.z,angles2.z,dt);

	//angles3.lerp(angles1,angles2,dt);
	////angle_lerp(angles3.y,angles1.y,angles2.y,dt);
	////angle_lerp(angles3.z,angles1.z,angles2.z,dt);
	//angle_lerp(angles3.x,angles1.x,angles2.x,dt);
	//XFORM().setHPB(angles3.x,angles3.y,angles3.z);
	Fvector position;
	position.set(Position());
	XFORM().rotation(q3);
	Position().set(position);
	return true;
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

	CWeapon *pWeapon = smart_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		Fvector dangle;
		dangle = pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso_yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso_pitch + dangle.x;
	}


	// если есть движение - выровнять модель по камере
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

	CWeapon *pWeapon = smart_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
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
	bool can_accel = !IsLimping() &&
		!m_PhysicMovementControl->PHCapture() &&
		/*(!(mstate_real&mcBack) || psActorFlags.test(AF_RUN_BACKWARD)) &&*/
		!m_bZoomAimingMode;

	return can_accel;
}

bool	CActor::CanJump				()
{
	bool can_Jump = !IsLimping() &&
		!m_PhysicMovementControl->PHCapture() &&((mstate_real&mcJump)==0) && (m_fJumpTime<=0.f) 
		&& !m_bJumpKeyPressed &&!m_bZoomAimingMode;

	return can_Jump;
}

#define CANT_WALK "cant_walk"
#define SHOW_CANT_WALK_TIME 5.f

bool	CActor::CanMove				()
{
	if(IsCantWalk())
	{
		static float m_fSignTime = 0.f;

		if(mstate_wishful&mcAnyMove && Device.fTimeGlobal - m_fSignTime > SHOW_CANT_WALK_TIME)
		{
			HUD().GetUI()->UIMainIngameWnd.AddInfoMessage(*CStringTable()(CANT_WALK));
			m_fSignTime = Device.fTimeGlobal;
		}
		return false;
	}

	if(IsTalking())
		return false;
	else
		return true;
}