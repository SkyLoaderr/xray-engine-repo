#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"
#include "../../../../skeletonanimated.h"

CSnork::CSnork() 
{
	StateMan = xr_new<CStateManagerSnork>	(this);
	
	CJumpingAbility::init_external			(this);
}

CSnork::~CSnork()
{
	xr_delete		(StateMan);
}

void CSnork::Load(LPCSTR section)
{
	inherited::Load			(section);
	CJumpingAbility::load	(section);

	MotionMan.accel_load			(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimStandIdle,	eAnimStandDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_damaged_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_damaged_",	-1,	&inherited::get_sd()->m_fsVelocityWalkFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_damaged_",	-1,	&inherited::get_sd()->m_fsVelocityRunFwdDamaged,	PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1,	&inherited::get_sd()->m_fsVelocityWalkFwdNormal,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::get_sd()->m_fsVelocityRunFwdNormal,		PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimDie,				"stand_die_",			0,  &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::get_sd()->m_fsVelocitySteal,			PS_STAND);
		MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::get_sd()->m_fsVelocityNone,				PS_STAND);


		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimEat);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
		MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle); 

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
		
		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

	m_fsVelocityJumpOne.Load(section, "Velocity_Jump_Stand");
	m_fsVelocityJumpTwo.Load(section, "Velocity_Jump_Forward");
}

void CSnork::reinit()
{
	inherited::reinit();
	
	CMotionDef			*def1, *def2, *def3;
	CSkeletonAnimated	*pSkel = smart_cast<CSkeletonAnimated*>(Visual());

	def1 = pSkel->ID_Cycle_Safe("stand_attack_2_0");	VERIFY(def1);
	def2 = pSkel->ID_Cycle_Safe("stand_attack_2_1");	VERIFY(def2);
	def3 = pSkel->ID_Cycle_Safe("stand_somersault_0");	VERIFY(def3);
	
	CJumpingAbility::reinit(def1, def2, def3);
	
	m_movement_params.insert(std::make_pair(eVelocityParameterJumpOne,	STravelParams(m_fsVelocityJumpOne.velocity.linear,	m_fsVelocityJumpOne.velocity.angular_path, m_fsVelocityJumpOne.velocity.angular_real)));
	m_movement_params.insert(std::make_pair(eVelocityParameterJumpTwo,	STravelParams(m_fsVelocityJumpTwo.velocity.linear,	m_fsVelocityJumpTwo.velocity.angular_path, m_fsVelocityJumpTwo.velocity.angular_real)));
}

void CSnork::UpdateCL()
{
	inherited::UpdateCL();
	CJumpingAbility::update_frame();
}

void CSnork::try_to_jump()
{
	CObject *target = const_cast<CEntityAlive *>(EnemyMan.get_enemy());
	if (!target || !EnemyMan.see_enemy_now()) return;

	if (CJumpingAbility::can_jump(target))
		CJumpingAbility::jump(target, eVelocityParamsJump);
}

void CSnork::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		MotionMan.SetCurAnim(eAnimLookAround);
		return;
	}
}

void CSnork::HitEntityInJump(const CEntity *pEntity)
{
	SAAParam params;
	MotionMan.AA_GetParams	(params, "stand_attack_2_1");
	HitEntity				(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}
