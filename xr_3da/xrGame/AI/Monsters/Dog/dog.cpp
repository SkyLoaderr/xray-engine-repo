#include "stdafx.h"
#include "dog.h"
#include "../ai_monster_utils.h"
#include "dog_state_manager.h"
#include "../../../../skeletonanimated.h"
#include "../ai_monster_movement.h"
#include "../../../level.h"
#include "../ai_monster_movement_space.h"

CAI_Dog::CAI_Dog()
{
	StateMan = xr_new<CStateManagerDog>(this);

	CJumping::Init		(this);
	CControlled::init_external(this);
}

CAI_Dog::~CAI_Dog()
{
	xr_delete(StateMan);
}

void CAI_Dog::reinit()
{
	inherited::reinit();

	Bones.Reset();
	strike_in_jump					= false;
}


void CAI_Dog::Load(LPCSTR section)
{
	inherited::Load	(section);
	CJumping::Load	(section);

	// todo: PUT visual from load OFF
	SVelocityParam &velocity_run		= movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	CJumping::AddState(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle_Safe("jump_glide_0"), JT_GLIDE,	false,	0.f, velocity_run.velocity.angular_real);

	MotionMan.AddReplacedAnim(&m_bDamaged,		eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged,		eAnimWalkFwd,	eAnimWalkDamaged);
	MotionMan.AddReplacedAnim(&m_bRunTurnLeft,	eAnimRun,		eAnimRunTurnLeft);
	MotionMan.AddReplacedAnim(&m_bRunTurnRight,	eAnimRun,		eAnimRunTurnRight);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	if (MotionMan.start_load_shared(CLS_ID)) {
		
		SVelocityParam &velocity_none		= movement().get_velocity(MonsterMovement::eVelocityParameterIdle);	
		SVelocityParam &velocity_turn		= movement().get_velocity(MonsterMovement::eVelocityParameterStand);
		SVelocityParam &velocity_walk		= movement().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
		SVelocityParam &velocity_run		= movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
		SVelocityParam &velocity_walk_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
		SVelocityParam &velocity_run_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
		SVelocityParam &velocity_steal		= movement().get_velocity(MonsterMovement::eVelocityParameterSteal);
		SVelocityParam &velocity_drag		= movement().get_velocity(MonsterMovement::eVelocityParameterDrag);

		// define animation set
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &velocity_none,		PS_LIE);
		MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &velocity_none,		PS_LIE);
		MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &velocity_none,		PS_SIT);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &velocity_walk_dmg,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&velocity_run,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1, &velocity_run_dmg,	PS_STAND);

		MotionMan.AddAnim(eAnimRunTurnLeft,		"stand_run_turn_left_",	-1, &velocity_run,		PS_STAND);
		MotionMan.AddAnim(eAnimRunTurnRight,	"stand_run_turn_right_",-1, &velocity_run,		PS_STAND);

		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &velocity_drag,		PS_STAND);
		//MotionMan.AddAnim(eAnimSniff,			"stand_sniff_",			-1, &velocity_none,		PS_STAND);
		//MotionMan.AddAnim(eAnimHowling,			"stand_howling_",		-1,	&velocity_none,		PS_STAND);
		//MotionMan.AddAnim(eAnimJumpGlide,		"jump_glide_",			-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,	PS_STAND);
		MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &velocity_none,		PS_STAND);

		MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, &velocity_none,		PS_SIT);
		MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,		PS_STAND);	
		MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,		PS_SIT);
		//MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1,	&velocity_none,		PS_LIE);
		MotionMan.AddAnim(eAnimLieSitUp,	"lie_to_sit_",		-1, &velocity_none,		PS_LIE);

		MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_left_",		-1, &velocity_none,		PS_STAND);
		MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_right_",	-1, &velocity_none,		PS_STAND);

		// define transitions
		// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
		MotionMan.AddTransition(PS_SIT,		PS_LIE,		eAnimSitLieDown,		false);
		MotionMan.AddTransition(PS_STAND,	PS_SIT,		eAnimStandSitDown,		false);
		MotionMan.AddTransition(PS_SIT,		PS_STAND,	eAnimSitStandUp,		false, SKIP_IF_AGGRESSIVE);
		MotionMan.AddTransition(PS_LIE,		PS_SIT,		eAnimLieSitUp,			false, SKIP_IF_AGGRESSIVE);
		
		// todo: stand -> lie

		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun); 
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);	
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);
		
		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();

	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}



void CAI_Dog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}
	
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
	}
}

void CAI_Dog::OnSoundPlay()
{
	
}

void CAI_Dog::UpdateCL()
{
	inherited::UpdateCL();

	CJumping::Update();
	float trace_dist = 1.0f;

	// ��������� �� ��������� ���� �� ����� ������
	if (CJumping::IsGlide()) {

		if (strike_in_jump) return;
		
		const CEntity *pE = smart_cast<const CEntity *>(CJumping::GetEnemy());
		if (!pE) return;

		Fvector trace_from;
		Center(trace_from);
		setEnabled(false);
		collide::rq_result	l_rq;

		if (Level().ObjectSpace.RayPick(trace_from, Direction(), trace_dist , collide::rqtBoth, l_rq)) {
			if ((l_rq.O == CJumping::GetEnemy()) && (l_rq.range < trace_dist)) {
				//HitEntity(pE, inherited::get_sd()->m_fHitPower,Direction());
				strike_in_jump = true;
			}
		}
		setEnabled(true);			

		// !!!
		// LookPosition(pE->Position());
	}
}

void CAI_Dog::OnJumpStop()
{
	MotionMan.DeactivateJump();
	MotionMan.Update();
	strike_in_jump = false;
}


//#define TURN_HEAD_ANGLE PI_DIV_4
//
//void CAI_Dog::BonesInMotion() 
//{
//	EMotionAnim anim = MotionMan.GetCurAnim();
//	bool b_enable_motions = false;
//
//	switch (anim) {
//	case eAnimSitIdle:
//	case eAnimWalkFwd:
//		b_enable_motions = true;
//		break;
//	}
//	
//
//	if (!Bones.IsActive() && b_enable_motions) {
//		float	x,y,z;
//		u8		side_to_look = 0; // 0 - front, 1 - left, 2 - right
//		
//		u8 selector = u8(Random.randI(100)); 
//		if (selector < 20)		side_to_look = 1;
//		else if (selector < 40)	side_to_look = 2;
//		
//		if (side_to_look == 1)	{	x = -TURN_HEAD_ANGLE;	y = TURN_HEAD_ANGLE;	z = TURN_HEAD_ANGLE;} 
//		else if (side_to_look == 2)	{	x = TURN_HEAD_ANGLE; 	y = TURN_HEAD_ANGLE;	z = -TURN_HEAD_ANGLE;}
//		else {x = 0.f; y = 0.f; z = 0.f;}
//
//		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_X, x, PI_DIV_2, 3000);
//		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Y, y, PI_DIV_2, 3000);
//		Bones.SetMotion(GetBoneInstance("bip01_head"),AXIS_Z, z, PI_DIV_2, 3000);
//	}
//}

