#include "stdafx.h"
#include "boar.h"
#include "../ai_monster_utils.h"
#include "boar_state_manager.h"
#include "../../../../skeletoncustom.h"
#include "../ai_monster_movement.h"

CAI_Boar::CAI_Boar()
{
	StateMan = xr_new<CStateManagerBoar>	(this);

	controlled::init_external(this);
}

CAI_Boar::~CAI_Boar()
{
	xr_delete(StateMan);
}



void CAI_Boar::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	if (MotionMan.start_load_shared(CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);

		MotionMan.AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE	);
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE	);

		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE	);
		MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE	);
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
		MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			 2, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_left_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_right_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimAttackRun,		"stand_run_attack_",	-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);

		// define transitions																											
		MotionMan.AddTransition(eAnimStandLieDown,	eAnimSleep,		eAnimLieToSleep,		false);										
		MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandLieDown,		true);
		MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandLieDown,		false);
		MotionMan.AddTransition(PS_LIE,				PS_STAND,		eAnimLieStandUp,		false, SKIP_IF_AGGRESSIVE);


		// define links from Action to animations
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimDragCorpse);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
		MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

		MotionMan.AA_Load	(pSettings->r_string(section, "attack_params"));
		//MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

void __stdcall CAI_Boar::BoneCallback(CBoneInstance *B)
{
	CAI_Boar	*P = static_cast<CAI_Boar*>(B->Callback_Param);

	if (!P->look_at_enemy) return;
	
	Fmatrix M;
	M.setXYZi (P->_cur_delta,0.0f, 0.0f);
	B->mTransform.mulB(M);
}

BOOL CAI_Boar::net_Spawn (CSE_Abstract* DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(smart_cast<CKinematics*>(Visual())->LL_BoneID("bip01_head"));
	BI.set_callback(BoneCallback,this);
	
	_cur_delta		= _target_delta = 0.f;
	_velocity		= PI;
	look_at_enemy	= false;
	return TRUE;
}


void CAI_Boar::LookPosition(Fvector to_point, float angular_speed)
{
	Fvector dir;
	dir.sub(to_point, Position());
	float yaw,pitch;
	dir.getHP(yaw,pitch);

	movement().m_body.target.yaw = angle_normalize(-yaw);
}


void CAI_Boar::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_ROTATION_JUMP) == ASP_ROTATION_JUMP) {
		float yaw, pitch;
		Fvector().sub(EnemyMan.get_enemy()->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;
		yaw = angle_normalize(yaw);

		EMotionAnim anim = eAnimJumpLeft;
		if (from_right(yaw,movement().m_body.current.yaw)) {
			anim = eAnimJumpRight;
			yaw = angle_normalize(yaw + PI / 20);	
		} else yaw = angle_normalize(yaw - PI / 20);

		MotionMan.Seq_Add(anim);
		MotionMan.Seq_Switch();

		movement().m_body.target.yaw = yaw;

		// calculate angular speed
		float new_angular_velocity; 
		float delta_yaw = angle_difference(yaw,movement().m_body.current.yaw);
		float time = MotionMan.GetCurAnimTime();
		new_angular_velocity = 2.5f * delta_yaw / time; 

		MotionMan.ForceAngularSpeed(new_angular_velocity);

		return;
	}

	if ((spec_params & ASP_ATTACK_RUN) == ASP_ATTACK_RUN) {
		MotionMan.SetCurAnim(eAnimAttackRun);
	}
}

void CAI_Boar::UpdateCL()
{
	inherited::UpdateCL();
	angle_lerp(_cur_delta, _target_delta, _velocity, Device.fTimeDelta);
}
