#include "stdafx.h"
#include "tushkano.h"
#include "tushkano_state_manager.h"
#include "../ai_monster_movement.h"
#include "../ai_monster_movement_space.h"

CTushkano::CTushkano()
{
	StateMan = xr_new<CStateManagerTushkano>(this);

	CControlled::init_external(this);
}

CTushkano::~CTushkano()
{
	xr_delete(StateMan);
}

void CTushkano::Load(LPCSTR section)
{
	inherited::Load	(section);

//	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
//	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	//MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	if (MotionMan.start_load_shared(CLS_ID)) {

		SVelocityParam &velocity_none		= movement().get_velocity(MonsterMovement::eVelocityParameterIdle);	
		SVelocityParam &velocity_turn		= movement().get_velocity(MonsterMovement::eVelocityParameterStand);
		SVelocityParam &velocity_walk		= movement().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
		SVelocityParam &velocity_run		= movement().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
		//SVelocityParam &velocity_walk_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
		//SVelocityParam &velocity_run_dmg	= movement().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
		//SVelocityParam &velocity_steal		= movement().get_velocity(MonsterMovement::eVelocityParameterSteal);
		//SVelocityParam &velocity_drag		= movement().get_velocity(MonsterMovement::eVelocityParameterDrag);


		MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,				PS_STAND);
		//MotionMan.AddAnim(eAnimStandDamaged,	"stand_idle_dmg_",		-1, &velocity_none,				PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_left_",		-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_right_",	-1, &velocity_turn,		PS_STAND);
		MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,	PS_STAND);
		//MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,	PS_STAND);
		MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&velocity_run,		PS_STAND);
		//MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,	PS_STAND);
		MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);
		//MotionMan.AddAnim(eAnimDie,				"stand_die_",			0,  &velocity_none,				PS_STAND);
		//MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&velocity_none,				PS_STAND);
		//MotionMan.AddAnim(eAnimSteal,			"stand_crawl_",			-1, &velocity_steal,			PS_STAND);
		//MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &velocity_none,				PS_SIT);
		//MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &velocity_none,				PS_SIT);
		//MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &velocity_none,				PS_STAND);
		//MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &velocity_none,				PS_STAND);
		//MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &velocity_none,				PS_SIT);

		//MotionMan.AddTransition(PS_STAND,		PS_SIT,		eAnimStandSitDown,	false);
		//MotionMan.AddTransition(PS_SIT,			PS_STAND,	eAnimSitStandUp,	false);

		MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
		MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
		MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
		MotionMan.LinkAction(ACT_RUN,			eAnimRun);
		MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_REST,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
		MotionMan.LinkAction(ACT_STEAL,			eAnimStandIdle);
		MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

		MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));

		MotionMan.finish_load_shared();
	}

#ifdef DEBUG	
	MotionMan.accel_chain_test		();
#endif

}

void CTushkano::CheckSpecParams(u32 spec_params)
{
	//if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
	//	MotionMan.Seq_Add(eAnimCheckCorpse);
	//	MotionMan.Seq_Switch();
	//}

	//if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
	//	MotionMan.SetCurAnim(eAnimLookAround);
	//	return;
	//}
}


