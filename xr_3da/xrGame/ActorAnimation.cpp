#include "stdafx.h"
#include "Actor.h"
#include "ActorAnimation.h"
#include "hudmanager.h"
#include "UI.h"
#include "weapon.h"
#include "inventory.h"
#include "missile.h"
#include "level.h"

static const float y_spin_factor		= 0.4f;
static const float y_shoulder_factor	= 0.4f;
static const float y_head_factor		= 0.2f;
static const float p_spin_factor		= 0.2f;
static const float p_shoulder_factor	= 0.7f;
static const float p_head_factor		= 0.1f;

void __stdcall CActor::SpinCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	VERIFY	(A);

	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_spin_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_spin_factor;

	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void __stdcall CActor::ShoulderCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	VERIFY(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_shoulder_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_shoulder_factor;

	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void __stdcall CActor::HeadCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	VERIFY(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw - A->r_model_yaw - A->r_model_yaw_delta)*y_head_factor;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*p_head_factor;
	
	Fvector c			= B->mTransform.c;
	spin.setXYZ			(-bone_pitch,bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void __stdcall CActor::VehicleHeadCallback(CBoneInstance* B)
{
	CActor*	A			= dynamic_cast<CActor*>(static_cast<CObject*>(B->Callback_Param));	VERIFY(A);
	Fmatrix				spin;
	float				bone_yaw	= angle_normalize_signed(A->r_torso.yaw)*0.75f;
	float				bone_pitch	= angle_normalize_signed(A->r_torso.pitch)*0.75f;
	
	Fvector c			= B->mTransform.c;
	spin.setXYZi		(-bone_pitch,-bone_yaw,0);
	B->mTransform.mulA_43(spin);
	B->mTransform.c		= c;
}

void ACTOR_DEFS::SActorMotions::SActorState::STorsoWpn::Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1)
{
	char			buf[128];
	aim				= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_aim_0"));
	holster			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_holster_0"));
	draw			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_draw_0"));
	reload			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_reload_0"));
	drop			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_drop_0"));
	attack			= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_0"));
	fire_idle		= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_1"));
	fire_end		= K->ID_Cycle_Safe(strconcat(buf,base0,"_torso",base1,"_attack_2"));
}
void ACTOR_DEFS::SActorMotions::SActorState::SAnimState::Create(CSkeletonAnimated* K, LPCSTR base0, LPCSTR base1)
{
	char			buf[128];
	legs_fwd		= K->ID_Cycle(strconcat(buf,base0,base1,"_fwd_0"));
	legs_back		= K->ID_Cycle(strconcat(buf,base0,base1,"_back_0"));
	legs_ls			= K->ID_Cycle(strconcat(buf,base0,base1,"_ls_0"));
	legs_rs			= K->ID_Cycle(strconcat(buf,base0,base1,"_rs_0"));
}
void ACTOR_DEFS::SActorMotions::SActorState::CreateClimb(CSkeletonAnimated* K)
{
	string128		buf,buf1;
	string16		base;
	
	//climb anims
	strcpy(base,"cl");
	legs_idle		= K->ID_Cycle(strconcat(buf,base,"_idle_1"));
	m_torso_idle	= K->ID_Cycle(strconcat(buf,base,"_torso_0_aim_0"));
	m_walk.Create	(K,base,"_run");
	m_run.Create	(K,base,"_run");

	//norm anims
	strcpy(base,"norm");
	legs_turn		= K->ID_Cycle(strconcat(buf,base,"_turn"));
	death			= K->ID_Cycle(strconcat(buf,base,"_death_0"));
	m_torso[0].Create(K,base,"_1");
	m_torso[1].Create(K,base,"_2");
	m_torso[2].Create(K,base,"_3");
	m_torso[3].Create(K,base,"_4");
	m_torso[4].Create(K,base,"_5");
	m_torso[5].Create(K,base,"_6");
	m_torso[6].Create(K,base,"_7");


	m_head_idle		= NULL;///K->ID_Cycle("head_idle_0");
	jump_begin		= K->ID_Cycle(strconcat(buf,base,"_jump_begin"));
	jump_idle		= K->ID_Cycle(strconcat(buf,base,"_jump_idle"));
	landing[0]		= K->ID_Cycle(strconcat(buf,base,"_jump_end"));
	landing[1]		= K->ID_Cycle(strconcat(buf,base,"_jump_end_1"));

	for (int k=0; k<12; ++k)
		m_damage[k]	= K->ID_FX(strconcat(buf,base,"_damage_",itoa(k,buf1,10)));
}
void ACTOR_DEFS::SActorMotions::SActorState::Create(CSkeletonAnimated* K, LPCSTR base)
{
	string128		buf,buf1;
	legs_turn		= K->ID_Cycle(strconcat(buf,base,"_turn"));
	legs_idle		= K->ID_Cycle(strconcat(buf,base,"_idle_1"));
	death			= K->ID_Cycle(strconcat(buf,base,"_death_0"));
	
	m_walk.Create	(K,base,"_walk");
	m_run.Create	(K,base,"_run");
	
	m_torso[0].Create(K,base,"_1");
	m_torso[1].Create(K,base,"_2");
	m_torso[2].Create(K,base,"_3");
	m_torso[3].Create(K,base,"_4");
	m_torso[4].Create(K,base,"_5");
	m_torso[5].Create(K,base,"_6");
	m_torso[6].Create(K,base,"_7");
	
	m_torso_idle	= K->ID_Cycle(strconcat(buf,base,"_torso_0_aim_0"));
	m_head_idle		= K->ID_Cycle("head_idle_0");
	jump_begin		= K->ID_Cycle(strconcat(buf,base,"_jump_begin"));
	jump_idle		= K->ID_Cycle(strconcat(buf,base,"_jump_idle"));
	landing[0]		= K->ID_Cycle(strconcat(buf,base,"_jump_end"));
	landing[1]		= K->ID_Cycle(strconcat(buf,base,"_jump_end_1"));

	for (int k=0; k<12; ++k)
		m_damage[k]	= K->ID_FX(strconcat(buf,base,"_damage_",itoa(k,buf1,10)));
}

void ACTOR_DEFS::SActorMotions::Create(CSkeletonAnimated* V)
{
	m_steering_torso_left	= V->ID_Cycle_Safe("steering_torso_ls");
	m_steering_torso_right	= V->ID_Cycle_Safe("steering_torso_rs");
	m_steering_torso_idle	= V->ID_Cycle_Safe("steering_torso_idle");
	m_steering_legs_idle	= V->ID_Cycle_Safe("steering_legs_idle");
	m_dead_stop				= V->ID_Cycle("norm_dead_stop_0");

	m_normal.Create	(V,"norm");
	m_crouch.Create	(V,"cr");
	//m_climb.Create	(V,"cr");
	m_climb.CreateClimb(V);
}
void CActor::steer_Vehicle(float angle)	
{
	if(!m_holder)		return;
	if(angle==0.f) 		PSkeletonAnimated	(Visual())->PlayCycle(m_anims.m_steering_torso_idle);
	else if(angle>0.f)	PSkeletonAnimated	(Visual())->PlayCycle(m_anims.m_steering_torso_right);
	else				PSkeletonAnimated	(Visual())->PlayCycle(m_anims.m_steering_torso_left);
}

void CActor::g_SetAnimation( u32 mstate_rl )
{
	if (g_Alive())
	{
		SActorMotions::SActorState*				ST = 0;
		SActorMotions::SActorState::SAnimState*	AS = 0;
		
		if		(mstate_rl&mcCrouch)	ST = &m_anims.m_crouch;
		else if	(mstate_rl&mcClimb)		ST = &m_anims.m_climb;
		else							ST = &m_anims.m_normal;

		if (isAccelerated(mstate_rl))	AS = &ST->m_run;
		else							AS = &ST->m_walk;
		
		// анимации
		CMotionDef* M_legs	= 0;
		CMotionDef* M_torso	= 0;
		CMotionDef* M_head	= 0;
		// Legs
		if		(mstate_rl&mcLanding)	M_legs	= ST->landing[0];
		else if (mstate_rl&mcLanding2)	M_legs	= ST->landing[1];
		else if ((mstate_rl&mcTurn)&&
				!(mstate_rl&mcClimb))	M_legs	= ST->legs_turn;
		else if (mstate_rl&mcFall)		M_legs	= ST->jump_idle;
		else if (mstate_rl&mcJump)		M_legs	= ST->jump_begin;
		else if (mstate_rl&mcFwd)		M_legs	= AS->legs_fwd;
		else if (mstate_rl&mcBack)		M_legs	= AS->legs_back;
		else if (mstate_rl&mcLStrafe)	M_legs	= AS->legs_ls;
		else if (mstate_rl&mcRStrafe)	M_legs	= AS->legs_rs;
	
		// Torso
		if(mstate_rl&mcClimb)
		{
			if		(mstate_rl&mcFwd)		M_torso	= AS->legs_fwd;
			else if (mstate_rl&mcBack)		M_torso	= AS->legs_back;
			else if (mstate_rl&mcLStrafe)	M_torso	= AS->legs_ls;
			else if (mstate_rl&mcRStrafe)	M_torso	= AS->legs_rs;
		}
		
		if(!M_torso)
		{
			CWeapon		*W = dynamic_cast<CWeapon*>(inventory().ActiveItem());
			CMissile	*M = dynamic_cast<CMissile*>(inventory().ActiveItem());
			if (W || M) {
				SActorMotions::SActorState::STorsoWpn* TW	= &ST->m_torso[(W ? W->HandDependence() : M->animation_slot()) - 1];
				if (!b_DropActivated&&!fis_zero(f_DropPower)){
					M_torso					= TW->drop;
					m_bAnimTorsoPlayed		= TRUE;
				}else{
					if (!m_bAnimTorsoPlayed) {
						if (W) {
							switch (W->STATE){
								case CWeapon::eIdle:	M_torso	= TW->aim;		break;
								case CWeapon::eFire:	M_torso	= TW->attack;	break;
								case CWeapon::eReload:	M_torso	= TW->reload;	break;
								case CWeapon::eShowing:	M_torso	= TW->draw;		break;
								case CWeapon::eHiding:	M_torso	= TW->holster;	break;
							}
						}
						else {
							switch (M->State()){
								case MS_SHOWING	 :		M_torso	= TW->draw;		break;
								case MS_HIDING	 :		M_torso	= TW->holster;	break;
								case MS_IDLE	 :		M_torso	= TW->aim;		break;
								case MS_EMPTY	 :		M_torso	= TW->aim;		break;
								case MS_THREATEN :		M_torso	= TW->attack;	break;
								case MS_READY	 :		M_torso	= TW->fire_idle;break;
								case MS_THROW	 :		M_torso	= TW->fire_end;	break;
								case MS_END		 :		M_torso	= TW->fire_end;	break;
							}
						}
					}
				}
			}
		}

		if (!M_legs)					M_legs	= ST->legs_idle;
		if (!M_head)					M_head	= ST->m_head_idle;
		if (!M_torso){				
			if (m_bAnimTorsoPlayed)		M_torso	= m_current_torso;
			else						M_torso = ST->m_torso_idle;
		}
		
		// есть анимация для всего - запустим / иначе запустим анимацию по частям
		if (m_current_torso!=M_torso){
			if (m_bAnimTorsoPlayed)		PSkeletonAnimated	(Visual())->PlayCycle(M_torso,TRUE,AnimTorsoPlayCallBack,this);
			else						PSkeletonAnimated	(Visual())->PlayCycle(M_torso);
			m_current_torso=M_torso;
		}
		if (m_current_legs!=M_legs){
			m_current_legs_blend = PSkeletonAnimated(Visual())->PlayCycle(M_legs);
			m_current_legs=M_legs;
		}

		if(m_current_head!=M_head)
		{
			if(M_head)PSkeletonAnimated(Visual())->PlayCycle(M_head);
			m_current_head=M_head;
		}

	}else{
		if (m_current_legs||m_current_torso){
			SActorMotions::SActorState* ST = 0;
			if (mstate_rl&mcCrouch)	ST = &m_anims.m_crouch;
			else					ST = &m_anims.m_normal;
			mstate_real			= 0;
			m_current_legs		= 0;
			m_current_torso		= 0;
			//PKinematics	(Visual())->PlayCycle(ST->death,false);
			//ST=&m_crouch;
		
			//PKinematics	(Visual())->PlayCycle(ST->death,false);
			//PKinematics	(Visual())->PlayCycle(ST->death);
			///////////////////////////
			//Render->model_Delete			(Visual());
			//Visual() = Render->model_Create  ("box_bone");
			//xr_delete                       (CFORM());
			//CFORM() = xr_new<CCF_Skeleton> (this);
			//CFORM()->OnMove();
			//PKinematics	(Visual())->PlayCycle("x90",false);
			////////////////////
	
			PSkeletonAnimated(Visual())->PlayCycle(m_anims.m_dead_stop);
		}
	}
#ifndef NDEBUG
#ifdef DEBUG
	string128 buf;
	strcpy(buf,"");
	if (mstate_rl&mcAccel)		strcat(buf,"Accel ");
	if (mstate_rl&mcCrouch)		strcat(buf,"Crouch ");
	if (mstate_rl&mcFwd)		strcat(buf,"Fwd ");
	if (mstate_rl&mcBack)		strcat(buf,"Back ");
	if (mstate_rl&mcLStrafe)	strcat(buf,"LStrafe ");
	if (mstate_rl&mcRStrafe)	strcat(buf,"RStrafe ");
	if (mstate_rl&mcJump)		strcat(buf,"Jump ");
	if (mstate_rl&mcFall)		strcat(buf,"Fall ");
	if (mstate_rl&mcTurn)		strcat(buf,"Turn ");
	if (mstate_rl&mcLanding)	strcat(buf,"Landing ");
	if (m_bJumpKeyPressed)		strcat(buf,"+Jumping ");
	HUD().pFontSmall->SetColor	(0xffffffff);
	HUD().pFontSmall->OutSet	(170,450);
	HUD().pFontSmall->OutNext	("MSTATE:     [%s]",buf);
//	if (buf[0]) 
//		Msg("%s",buf);
	switch (m_PhysicMovementControl->Environment())
	{
	case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
	case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
	case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
	}
	HUD().pFontSmall->OutNext	(buf);
	HUD().pFontSmall->OutNext	("Accel     [%3.2f, %3.2f, %3.2f]",VPUSH(NET_SavedAccel));
	HUD().pFontSmall->OutNext	("V         [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
	HUD().pFontSmall->OutNext	("vertex ID   %d",level_vertex_id());
#endif
#endif
}

