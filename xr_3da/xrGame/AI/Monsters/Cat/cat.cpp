#include "stdafx.h"
#include "cat.h"
#include "cat_state_manager.h"
#include "../../../../skeletonanimated.h"
#include "../ai_monster_utils.h"
#include "../ai_monster_movement.h"

CCat::CCat()
{
	StateMan = xr_new<CStateManagerCat>		(this);
	
	CJumpingAbility::init_external			(this);
}

CCat::~CCat()
{
	xr_delete(StateMan);
}

void CCat::Load(LPCSTR section)
{
	inherited::Load			(section);
	CJumpingAbility::load	(section);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	//*****************************************************************************
	// load shared motion data
	//*****************************************************************************

	if (MotionMan.start_load_shared(CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_dmg_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimAttackRat,		"stand_attack_rat_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);		
		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
		MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);		
		
		MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_ls_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_rs_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		
		MotionMan.AddTransition(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);
		MotionMan.AddTransition(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);


		// link action
		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimLieIdle);
		MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

	//*****************************************************************************
}

void CCat::reinit()
{
	inherited::reinit();

	CMotionDef			*def1, *def2, *def3;
	CSkeletonAnimated	*pSkel = smart_cast<CSkeletonAnimated*>(Visual());

	def1 = pSkel->ID_Cycle_Safe("jump_attack_0");	VERIFY(def1);
	def2 = pSkel->ID_Cycle_Safe("jump_attack_1");	VERIFY(def2);
	def3 = pSkel->ID_Cycle_Safe("jump_attack_2");	VERIFY(def3);

	CJumpingAbility::reinit(def1, def2, def3);
}

void CCat::try_to_jump()
{
	CObject *target = const_cast<CEntityAlive *>(EnemyMan.get_enemy());
	if (!target || !EnemyMan.see_enemy_now()) return;

	if (CJumpingAbility::can_jump(target))
		CJumpingAbility::jump(target);
}

void CCat::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add	(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_ATTACK_RAT) == ASP_ATTACK_RAT) MotionMan.SetCurAnim(eAnimAttackRat);

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

		movement().stop_linear		();
		movement().m_body.target.yaw = yaw;

		// calculate angular speed
		float new_angular_velocity; 
		float delta_yaw = angle_difference(yaw,movement().m_body.current.yaw);
		float time = MotionMan.GetCurAnimTime();
		new_angular_velocity = delta_yaw / time; 

		MotionMan.ForceAngularSpeed(new_angular_velocity);

		return;
	}


}

void CCat::UpdateCL()
{
	inherited::UpdateCL				();
	CJumpingAbility::update_frame	();
}

void CCat::HitEntityInJump(const CEntity *pEntity)
{
	SAAParam params;
	MotionMan.AA_GetParams	(params, "jump_attack_2");
	HitEntity				(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}


