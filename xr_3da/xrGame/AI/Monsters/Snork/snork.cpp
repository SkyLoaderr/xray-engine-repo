#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"
#include "../../../../skeletonanimated.h"

CSnork::CSnork() 
{
	StateMan = xr_new<CStateManagerSnork>(this);

	EventMan.get_binder(eventAnimationStart)->bind	(this, &CSnork::on_test_1);
	EventMan.get_binder(eventAnimationStart)->bind	(this, &CSnork::on_test_2);
	EventMan.get_binder(eventAnimationStart)->bind	(this, &CSnork::on_test_3);
	EventMan.get_binder(eventAnimationStart)->bind	(this, &CSnork::on_test_2);

	EventMan.get_binder(eventSoundStart)->bind	(this, &CSnork::on_test_1);
	EventMan.get_binder(eventSoundStart)->bind	(this, &CSnork::on_test_2);

}

CSnork::~CSnork()
{
	xr_delete		(StateMan);
}

void CSnork::Load(LPCSTR section)
{
	inherited::Load	(section);

	MotionMan.accel_load			(section);

	if (MotionMan.start_load_shared(SUB_CLS_ID)) {
		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::get_sd()->m_fsVelocityNone,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::get_sd()->m_fsVelocityStandTurn,		PS_STAND);
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
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimRun);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimRun);
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

}

void CSnork::reinit()
{
	inherited::reinit();
	
	CMotionDef			*def1, *def2, *def3;
	CSkeletonAnimated	*pSkel = smart_cast<CSkeletonAnimated*>(Visual());

	def1 = pSkel->ID_Cycle_Safe("stand_attack_2_0");	VERIFY(def1);
	def2 = pSkel->ID_Cycle_Safe("stand_attack_2_1");	VERIFY(def2);
	def3 = pSkel->ID_Cycle_Safe("stand_run_0");			VERIFY(def3);
	
	CJumpingAbility::init_external(this, def1, def2, def3);
}

void CSnork::UpdateCL()
{
	inherited::UpdateCL();
	CJumpingAbility::update_frame();
}

void CSnork::test()
{
	//CJumpingAbility::jump(CJumpingAbility::get_target(Level().CurrentEntity()), eVelocityParameterRunNormal);

	EventMan.raise(eventSoundStart);
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

void CSnork::test2()
{
	//Fvector target_position;
	//Fvector dir;
	//dir.set(Direction());
	//dir.invert();
	//target_position.mad(Position(), dir, 5.f);
	//
	//bool ret_val = CMonsterMovement::build_special(target_position, u32(-1), eVelocityParameterRunNormal);
	//if (!ret_val) Msg("SPECIAL FAILED!!!");
	//else Msg("SPECIAL PATH BUILT!!!");

	EventMan.raise(eventAnimationStart);
}

void CSnork::on_test_1(IEventData *data)
{
	Msg("DELEGATE test1");
}
void CSnork::on_test_2(IEventData *data)
{	
	Msg("DELEGATE test2");
}
void CSnork::on_test_3(IEventData *data)
{
	Msg("DELEGATE test3");
}


