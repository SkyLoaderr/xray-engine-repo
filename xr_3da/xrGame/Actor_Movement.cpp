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
#include "actorcondition.h"
#include "game_cl_base.h"

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
	// Lookout
	if (mstate_real&mcAnyAction){
		mstate_real			&= ~mcLookout;
	}else{
		if (mstate_wf&mcLookout)	mstate_real		|= mstate_wf&mcLookout;
		else						mstate_real		&= ~mcLookout;
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
		if(!(mstate_real & mcClimb))
		{
			mstate_real				|=mcClimb;
			mstate_real				&=~mcSprint;
			cam_SetLadder();
		}
	}
	else
	{
		if (mstate_real & mcClimb)
		{
			cam_UnsetLadder();
		}
		mstate_real				&=~mcClimb;		
	};

	// изменилось состояние
	if (mstate_wf != mstate_real){
		if ((mstate_real&mcCrouch)&&((0==(mstate_wf&mcCrouch)) || mstate_real&mcClimb)){
			if (ActivateBox(0)){
				mstate_real &= ~mcCrouch;
			}
		}
	}

	if(!CanAccelerate()&&isAccelerated(mstate_real))
	{
		mstate_real				^=mcAccel; // toggle accel
	};	

	if (this == Level().CurrentControlEntity())
	{
		if ((mstate_real&mcClimb) != (mstate_old&mcClimb))
		{
			if (mstate_real&mcClimb)
			{
				HideCurrentWeapon(GEG_PLAYER_DEACTIVATE_CURRENT_SLOT);//, true);
			}
			else
			{
				RestoreHidedWeapon(GEG_PLAYER_RESTORE_CURRENT_SLOT);
			};			
		}
		else
		{
			if (mstate_real & mcClimb)
			{
				 if ((mstate_real&mcAnyMove) != (mstate_old&mcAnyMove))
				 {
					 if (mstate_real&mcAnyMove)
					 {
						 HideCurrentWeapon(GEG_PLAYER_DEACTIVATE_CURRENT_SLOT);//, true);
					 }
					 else
					 {
						 RestoreHidedWeapon(GEG_PLAYER_RESTORE_CURRENT_SLOT);
					 };
				 }
			}
		}

		//-----------------------------------------------------------
		if ((mstate_real&mcSprint) != (mstate_old & mcSprint))
		{
			if (mstate_real&mcSprint)
			{
				//HideCurrentWeapon(GEG_PLAYER_SPRINT_START);//, false);
				psHUD_Flags.set(HUD_WEAPON_RT,FALSE);
				psHUD_Flags.set(HUD_CROSSHAIR_RT,FALSE);

			}
			else
			{
				//RestoreHidedWeapon(GEG_PLAYER_SPRINT_END);
				psHUD_Flags.set(HUD_WEAPON_RT,TRUE);
				psHUD_Flags.set(HUD_CROSSHAIR_RT,TRUE);
			}
		};
	};
};

void CActor::g_cl_CheckControls(u32 mstate_wf, Fvector &vControlAccel, float &Jump, float dt)
{
	mstate_old = mstate_real;
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
			if (!GodMode())//(!psActorFlags.test(AF_GODMODE))	
				conditions().ConditionJump(inventory().TotalWeight()/
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
			bool Crouched = false;
			if (isAccelerated(mstate_wf))
				Crouched = ActivateBox(1);
			else
				Crouched = ActivateBox(2);
			if (Crouched) mstate_real			|=	mcCrouch;
		}

		// mask input into "real" state
		u32 move	= mcAnyMove|mcAccel;

		if (((mstate_real&mcCrouch)))
		{
			if (!isAccelerated(mstate_real) && isAccelerated(mstate_wf))
//			if(0==(mstate_real&mcAccel)&&(mstate_wf&mcAccel))
			{
				m_PhysicMovementControl->EnableCharacter();
				if(!ActivateBox(1))move	&=~mcAccel;
			}

			if (isAccelerated(mstate_real) && !isAccelerated(mstate_wf))
//			if((mstate_real&mcAccel)&&0==(mstate_wf&mcAccel))
			{
				m_PhysicMovementControl->EnableCharacter();
				if(ActivateBox(2))mstate_real	&=~mcAccel;
			}

		}

		if (!CanSprint())
		{
			mstate_wf				&= ~mcSprint;
		}

		mstate_real &= (~move);
		mstate_real |= (mstate_wf & move);

		if(mstate_wf&mcSprint)
			mstate_real|=mcSprint;
		else
			mstate_real&=~mcSprint;
		if(!(mstate_real&(mcFwd|mcLStrafe|mcRStrafe))||mstate_real&(mcCrouch|mcClimb)|| !isAccelerated(mstate_wf))
		{
			mstate_real&=~mcSprint;
			mstate_wishful&=~mcSprint;
		}
				
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
				if (mstate_real&mcSprint)	scale *= m_fSprintFactor;
				vControlAccel.mul			(scale);
			}else{
				//				mstate_real	&= ~mcAnyMove;
			}
		}		
	}else{
		//		mstate_real			&=~ mcAnyMove;
	}

	//-------------------------------------------------------------------------------	
	

	//transform local dir to world dir
	Fmatrix				mOrient;
	mOrient.rotateY		(-r_model_yaw);
	mOrient.transform_dir(vControlAccel);
	//XFORM().transform_dir(vControlAccel);

	/*
	if(mstate_real&mcClimb&&mstate_real&mcAnyMove&&
	inventory().ActiveItem()&&inventory().ActiveItem()->HandDependence()==hd2Hand)
	{
		//inventory().ActiveItem()->Deactivate();
		inventory().Activate(NO_ACTIVE_SLOT);
	}
*/
}

#define ACTOR_ANIM_SECT "actor_animation"

void CActor::g_Orientate	(u32 mstate_rl, float dt)
{
	static float fwd_l_strafe_yaw	= deg2rad(pSettings->r_float(ACTOR_ANIM_SECT,	"fwd_l_strafe_yaw"));
	static float back_l_strafe_yaw	= deg2rad(pSettings->r_float(ACTOR_ANIM_SECT,	"back_l_strafe_yaw"));
	static float fwd_r_strafe_yaw	= deg2rad(pSettings->r_float(ACTOR_ANIM_SECT,	"fwd_r_strafe_yaw"));
	static float back_r_strafe_yaw	= deg2rad(pSettings->r_float(ACTOR_ANIM_SECT,	"back_r_strafe_yaw"));
	static float l_strafe_yaw		= deg2rad(pSettings->r_float(ACTOR_ANIM_SECT,	"l_strafe_yaw"));
	static float r_strafe_yaw		= deg2rad(pSettings->r_float(ACTOR_ANIM_SECT,	"r_strafe_yaw"));

	if(!g_Alive())return;
	// visual effect of "fwd+strafe" like motion
	float calc_yaw = 0;
	if(mstate_real&mcClimb)
	{
		if(g_LadderOrient()) return;
	}
	switch(mstate_rl&mcAnyMove)
	{
	case mcFwd+mcLStrafe:
		calc_yaw = +fwd_l_strafe_yaw;//+PI_DIV_4; 
		break;
	case mcBack+mcRStrafe:
		calc_yaw = +back_r_strafe_yaw;//+PI_DIV_4; 
		break;
	case mcFwd+mcRStrafe:
		calc_yaw = -fwd_r_strafe_yaw;//-PI_DIV_4; 
		break;
	case mcBack+mcLStrafe: 
		calc_yaw = -back_l_strafe_yaw;//-PI_DIV_4; 
		break;
	case mcLStrafe:
		calc_yaw = +l_strafe_yaw;//+PI_DIV_3-EPS_L; 
		break;
	case mcRStrafe:
		calc_yaw = -r_strafe_yaw;//-PI_DIV_4+EPS_L; 
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
bool CActor::g_LadderOrient()
{
	Fvector leader_norm;
	m_PhysicMovementControl->GroundNormal(leader_norm);
	if(_abs(leader_norm.y)>M_SQRT1_2) return false;
	//leader_norm.y=0.f;
	float mag=leader_norm.magnitude();
	if(mag<EPS_L) return false;
	leader_norm.div(mag);
	leader_norm.invert();
	Fmatrix M;M.set(Fidentity);
	M.k.set(leader_norm);
	M.j.set(0.f,1.f,0.f);
	generate_orthonormal_basis1(M.k,M.j,M.i);
	M.i.invert();
	//M.j.invert();


	//Fquaternion q1,q2,q3;
	//q1.set(XFORM());
	//q2.set(M);
	//q3.slerp(q1,q2,dt);
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
	//XFORM().rotation(q3);
	VERIFY2(_valid(M),"Invalide matrix in g_LadderOrient");
	XFORM().set(M);
	VERIFY2(_valid(position),"Invalide position in g_LadderOrient");
	Position().set(position);
	return true;
}
// ****************************** Update actor orientation according to camera orientation
void CActor::g_cl_Orientate	(u32 mstate_rl, float dt)
{
	// capture camera into torso (only for FirstEye & LookAt cameras)
	float tgt_roll		=	0.f;
	if (eacFreeLook!=cam_active){
		r_torso.yaw		=	cam_Active()->GetWorldYaw	();
		r_torso.pitch	=	cam_Active()->GetWorldPitch	();

		unaffected_r_torso.yaw		= r_torso.yaw;
		unaffected_r_torso.pitch	= r_torso.pitch;
		unaffected_r_torso.roll		= r_torso.roll;
	}

	CWeapon *pWeapon = smart_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon){
		Fvector			dangle;
		dangle			= pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso.yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso.pitch + dangle.x;
	}

	if (mstate_rl&mcLookout)
		tgt_roll		=	(mstate_rl&mcLLookout)?-ACTOR_LOOKOUT_ANGLE:ACTOR_LOOKOUT_ANGLE;
	if (!fsimilar(tgt_roll,r_torso_tgt_roll,EPS)){
		angle_lerp		(r_torso_tgt_roll,tgt_roll,PI_MUL_2,dt);
		r_torso_tgt_roll= angle_normalize_signed(r_torso_tgt_roll);
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
	r_torso.roll	= NET_Last.o_torso.roll;

	CWeapon *pWeapon = smart_cast<CWeapon*>(inventory().GetActiveSlot() != NO_ACTIVE_SLOT ? 
		inventory().m_slots[inventory().GetActiveSlot()].m_pIItem : NULL);

	if(pWeapon) 
	{
		Fvector dangle;
		dangle = pWeapon->GetRecoilDeltaAngle();
		r_torso.yaw		=	unaffected_r_torso.yaw + dangle.y;
		r_torso.pitch	=	unaffected_r_torso.pitch + dangle.x;
		r_torso.roll	=	unaffected_r_torso.roll + dangle.z;
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
	bool can_accel = !conditions().IsLimping() &&
		!m_PhysicMovementControl->PHCapture() &&
		/*(!(mstate_real&mcBack) || psActorFlags.test(AF_RUN_BACKWARD)) &&*/
		!m_bZoomAimingMode;

	return can_accel;
}

bool CActor::CanSprint			()
{
	bool can_Sprint = CanAccelerate() && !conditions().IsCantSprint() &&
						Game().PlayerCanSprint(this);

	return can_Sprint;
}

bool	CActor::CanJump				()
{
	bool can_Jump = /*!IsLimping() &&*/
		!m_PhysicMovementControl->PHCapture() &&((mstate_real&mcJump)==0) && (m_fJumpTime<=0.f) 
		&& !m_bJumpKeyPressed &&!m_bZoomAimingMode;

	return can_Jump;
}

#define CANT_WALK "cant_walk"
#define SHOW_CANT_WALK_TIME 5.f

bool	CActor::CanMove				()
{
	if(conditions().IsCantWalk())
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