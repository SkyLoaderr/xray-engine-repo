#include "stdafx.h"
#include "chimera.h"
#include "chimera_state_manager.h"
#include "../../../../skeletonanimated.h"
#include "../../../detail_path_manager.h"
#include "../ai_monster_movement.h"

CChimera::CChimera()
{
	StateMan = xr_new<CStateManagerChimera>	(this);

	CJumpingAbility::init_external			(this);
}

CChimera::~CChimera()
{
	xr_delete(StateMan);
}
void CChimera::Load(LPCSTR section)
{
	inherited::Load			(section);
	CJumpingAbility::load	(section);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	//*****************************************************************************
	// load shared motion data
	//*****************************************************************************

	if (MotionMan.start_load_shared(CLS_ID)) {

		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);

		MotionMan.AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);
		MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_LIE);

		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);

		MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::get_sd()->m_fsVelocityDrag,				PS_STAND);
		MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			 2, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);

		MotionMan.AddAnim(eAnimAttackRun,		"stand_run_attack_",	-1, &inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);

		//////////////////////////////////////////////////////////////////////////

		MotionMan.AddAnim(eAnimUpperStandIdle,		"stand_up_idle_",		-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND_UPPER);
		MotionMan.AddAnim(eAnimUpperStandTurnLeft,	"stand_up_turn_ls_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,	PS_STAND_UPPER);
		MotionMan.AddAnim(eAnimUpperStandTurnRight,	"stand_up_turn_rs_",	-1, &inherited::get_sd()->m_fsVelocityStandTurn,	PS_STAND_UPPER);

		MotionMan.AddAnim(eAnimStandToUpperStand,	"stand_upper_",			-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND);
		MotionMan.AddAnim(eAnimUppperStandToStand,	"stand_up_to_down_",	-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND_UPPER);

		MotionMan.AddAnim(eAnimUpperWalkFwd,		"stand_up_walk_fwd_",	-1, &m_fsVelocityWalkUpper,							PS_STAND_UPPER);
		MotionMan.AddAnim(eAnimUpperThreaten,		"stand_up_threaten_",	-1, &inherited::get_sd()->m_fsVelocityNone,			PS_STAND_UPPER);
		MotionMan.AddAnim(eAnimUpperAttack,			"stand_up_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,	PS_STAND_UPPER);

		//////////////////////////////////////////////////////////////////////////
		// define transitions
		MotionMan.AddTransition(PS_STAND,			PS_STAND_UPPER,		eAnimStandToUpperStand,		false);
		MotionMan.AddTransition(PS_STAND_UPPER,		PS_STAND,			eAnimUppperStandToStand,	false);

		// link action
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

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

	//*****************************************************************************

	m_fsVelocityWalkUpper.Load	(section, "Velocity_Walk_Upper");
	m_fsVelocityJumpOne.Load	(section, "Velocity_Jump_Stand");
	m_fsVelocityJumpTwo.Load	(section, "Velocity_Jump_Forward");
}

void CChimera::reinit()
{
	inherited::reinit();
	b_upper_state					= false;

	movement().detail().add_velocity(eVelocityParameterUpperWalkFwd,	CDetailPathManager::STravelParams(m_fsVelocityWalkUpper.velocity.linear,	m_fsVelocityWalkUpper.velocity.angular_path, m_fsVelocityWalkUpper.velocity.angular_real));
	movement().detail().add_velocity(eVelocityParameterJumpOne,	CDetailPathManager::STravelParams(m_fsVelocityJumpOne.velocity.linear,	m_fsVelocityJumpOne.velocity.angular_path, m_fsVelocityJumpOne.velocity.angular_real));
	movement().detail().add_velocity(eVelocityParameterJumpTwo,	CDetailPathManager::STravelParams(m_fsVelocityJumpTwo.velocity.linear,	m_fsVelocityJumpTwo.velocity.angular_path, m_fsVelocityJumpTwo.velocity.angular_real));

	CMotionDef			*def1, *def2, *def3;
	CSkeletonAnimated	*pSkel = smart_cast<CSkeletonAnimated*>(Visual());

	def1 = pSkel->ID_Cycle_Safe("jump_attack_0");	VERIFY(def1);
	def2 = pSkel->ID_Cycle_Safe("jump_attack_1");	VERIFY(def2);
	def3 = pSkel->ID_Cycle_Safe("jump_attack_2");	VERIFY(def3);

	CJumpingAbility::reinit(def1, def2, def3);
}

void CChimera::SetTurnAnimation(bool turn_left)
{
	if (b_upper_state) 
		(turn_left) ? MotionMan.SetCurAnim(eAnimUpperStandTurnLeft) : MotionMan.SetCurAnim(eAnimUpperStandTurnRight);
	else 
		(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft)		: MotionMan.SetCurAnim(eAnimStandTurnRight);
}

void CChimera::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		if (b_upper_state)
			MotionMan.SetCurAnim(eAnimUpperThreaten);
		else 
			MotionMan.SetCurAnim(eAnimThreaten);
	}

	if ((spec_params & ASP_ATTACK_RUN) == ASP_ATTACK_RUN) {
		MotionMan.SetCurAnim(eAnimAttackRun);
	}

	if (b_upper_state) {
		switch (MotionMan.GetCurAnim()) {
			case eAnimAttack:			MotionMan.SetCurAnim(eAnimUpperAttack);			break;
			case eAnimRun:
			case eAnimWalkFwd:			MotionMan.SetCurAnim(eAnimUpperWalkFwd);		break;
			case eAnimStandTurnLeft:	MotionMan.SetCurAnim(eAnimUpperStandTurnLeft);	break;
			case eAnimStandTurnRight:	MotionMan.SetCurAnim(eAnimUpperStandTurnRight); break;
			case eAnimThreaten:			MotionMan.SetCurAnim(eAnimUpperThreaten);		break;
			case eAnimStandIdle:		MotionMan.SetCurAnim(eAnimUpperStandIdle);		break;
		}
	}
}

EAction CChimera::CustomVelocityIndex2Action(u32 velocity_index) 
{
	switch (velocity_index) {
		case eVelocityParameterUpperWalkFwd: return ACT_WALK_FWD;
	}
	
	return ACT_STAND_IDLE;
}

void CChimera::TranslateActionToPathParams()
{
	if (!movement().b_enable_movement) return;

	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (MotionMan.m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
		bEnablePath = false;
		break;
	case ACT_WALK_FWD:
		if (b_upper_state) {
			vel_mask = eVelocityParamsUpperWalkFwd;
			des_mask = eVelocityParameterUpperWalkFwd;
		} else {
			if (m_bDamaged) {
				vel_mask = eVelocityParamsWalkDamaged;
				des_mask = eVelocityParameterWalkDamaged;
			} else {
				vel_mask = eVelocityParamsWalk;
				des_mask = eVelocityParameterWalkNormal;
			}
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (b_upper_state) {
			vel_mask = eVelocityParamsUpperWalkFwd;
			des_mask = eVelocityParameterUpperWalkFwd;
		} else {
			if (m_bDamaged) {
				vel_mask = eVelocityParamsRunDamaged;
				des_mask = eVelocityParameterRunDamaged;
			} else {
				vel_mask = eVelocityParamsRun;
				des_mask = eVelocityParameterRunNormal;
			}
		}
		break;
	case ACT_DRAG:
		vel_mask = eVelocityParameterDrag;
		des_mask = eVelocityParameterDrag;

		MotionMan.SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = eVelocityParameterSteal;
		des_mask = eVelocityParameterSteal;
		break;
	}

	if (force_real_speed) vel_mask = des_mask;

	if (bEnablePath) {
		movement().detail().set_velocity_mask	(vel_mask);	
		movement().detail().set_desirable_mask	(des_mask);
		movement().enable_path	();		
	} else {
		movement().disable_path	();
	}
}

void CChimera::HitEntityInJump(const CEntity *pEntity)
{
	SAAParam params;
	MotionMan.AA_GetParams	(params, "jump_attack_1");
	HitEntity				(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}

void CChimera::UpdateCL()
{
	inherited::UpdateCL				();
	CJumpingAbility::update_frame	();
}

void CChimera::try_to_jump()
{
	CObject *target = const_cast<CEntityAlive *>(EnemyMan.get_enemy());
	if (!target || !EnemyMan.see_enemy_now()) return;

	if (CJumpingAbility::can_jump(target))
		CJumpingAbility::jump(target, eVelocityParamsJump);
}

