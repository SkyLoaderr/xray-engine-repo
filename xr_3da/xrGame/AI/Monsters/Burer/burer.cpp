#include "stdafx.h"
#include "burer.h"
#include "../../ai_monster_utils.h"
#include "burer_states.h"

CBurer::CBurer() : CStateManagerBurer("Burer State Manager")
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBurerAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this);
	stateNull			= xr_new<CBitingNull>		();

	CurrentState		= stateRest;

	Init();
}

CBurer::~CBurer()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
	xr_delete(stateNull);

}


void CBurer::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}

void CBurer::reinit()
{
	inherited::reinit			();
	CStateManagerBurer::reinit	(this);	
	
}


void CBurer::reload(LPCSTR section)
{
	inherited::reload(section);

	// Load triple gravi animations
	CMotionDef *def1, *def2, *def3;
	def1 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_gravi_0");
	VERIFY(def1);

	def2 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_gravi_1");
	VERIFY(def2);

	def3 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_gravi_2");
	VERIFY(def3);

	anim_triple_gravi.init_external	(def1, def2, def3);
	
	// Load triple tele animations
	def1 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_tele_0");
	VERIFY(def1);

	def2 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_tele_1");
	VERIFY(def2);

	def3 = PSkeletonAnimated(Visual())->ID_Cycle_Safe("stand_tele_2");
	VERIFY(def3);

	anim_triple_tele.init_external	(def1, def2, def3);

}

void CBurer::Load(LPCSTR section)
{
	inherited::Load				(section);
	CStateManagerBurer::Load	(section);

	BEGIN_LOAD_SHARED_MOTION_DATA();
	
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLieIdle,			"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);

	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);

//	MotionMan.AddAnim(eAnimTeleRaise,		"",						-1, );
//	MotionMan.AddAnim(eAnimTeleFire);
//	MotionMan.AddAnim(eAnimGraviPrepare);
//	MotionMan.AddAnim(eAnimGraviFire);


	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun	, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);


}

void CBurer::StateSelector()
{	
	IState *state = 0;

	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong:				state = stateAttack; break;
			case eStrong:		
			case eNormal:
			case eWeak:						state = stateAttack; break;
		}
	} else if (hear_dangerous_sound || hear_interesting_sound) {
		if (hear_dangerous_sound)			state = stateNull;		
		if (hear_interesting_sound)			state = stateNull;	
	} else									state = stateNull; 

	SetState(state); 

	
}

void CBurer::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) {
		//m_body.current.yaw = m_body.target.yaw;
		return;
	}

	EMotionAnim anim = MotionMan.GetCurAnim();

	bool turn_left = true;
	if (from_right(m_body.target.yaw, m_body.current.yaw)) turn_left = false; 

	switch (anim) {
		case eAnimStandIdle: 
			(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			return;
		default:
			if (delta_yaw > deg(30)) {
				(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			}
			return;
	}
}

bool CBurer::UpdateStateManager()
{
	//CStateManagerBurer::update(m_current_update - m_dwLastUpdateTime);
	return false;
}


void CBurer::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	TTelekinesis::schedule_update();
}

void CBurer::CheckSpecParams(u32 spec_params)
{
//	if ((spec_params & ASP_GRAVI_ATTACK) == ASP_GRAVI_ATTACK) {
//		//MotionMan.AT_Activate(&anim_triple_gravi);
//	}
}




