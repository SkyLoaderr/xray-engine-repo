#include "stdafx.h"
#include "ai_boar.h"
#include "../ai_monster_utils.h"

CAI_Boar::CAI_Boar()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this, false);
	stateNull			= xr_new<CBitingNull>		();

	CurrentState		= stateRest;

	Init();
}

CAI_Boar::~CAI_Boar()
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


void CAI_Boar::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

}

void CAI_Boar::Load(LPCSTR section)
{
	inherited::Load	(section);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);

	MotionMan.AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::_sd->m_fsVelocityWalkFwdDamaged,	PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::_sd->m_fsVelocityRunFwdDamaged,		PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimLieToSleep,		"lie_to_sleep_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::_sd->m_fsVelocityDrag,				PS_STAND);
	MotionMan.AddAnim(eAnimLookAround,		"stand_idle_",			 2, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::_sd->m_fsVelocitySteal,				PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND);
	MotionMan.AddAnim(eAnimJumpLeft,		"stand_jump_left_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);
	MotionMan.AddAnim(eAnimJumpRight,		"stand_jump_right_",	-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND);

	// define transitions
	MotionMan.AddTransition(eAnimStandLieDown,	eAnimSleep,		eAnimLieToSleep,		false);
	MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandLieDown,		true);
	MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandLieDown,		false);
	MotionMan.AddTransition(PS_LIE,				PS_STAND,		eAnimLieStandUp,		false);

	

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimDragCorpse);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun	, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

	Fvector center;
	center.set		(0.f,0.f,0.f);

//	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 500,	600,	Fvector().set(0.f,0.f,0.f),		Fvector().set(0.f,0.f,2.3f), inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));
//	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 500,	600,	Fvector().set(0.f,0.f,0.f),		Fvector().set(0.f,0.f,2.3f), inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));
//	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 500,	600,	Fvector().set(0.f,0.f,0.f),		Fvector().set(0.f,0.f,3.0f), inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));

	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 0, 500,	600,	STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 1, 500,	600,	STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttack, 2, 500,	600,	STANDART_ATTACK, inherited::_sd->m_fHitPower,Fvector().set(0.f,0.f,3.f));

	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

}


void CAI_Boar::StateSelector()
{	

	IState *state;
	VisionElem ve;

	if (C && H && I)			state = statePanic;
	else if (C && H && !I)		state = statePanic;
	else if (C && !H && I)		state = statePanic;
	else if (C && !H && !I) 	state = statePanic;
	else if (D && H && I)		state = stateAttack;
	else if (D && H && !I)		state = stateAttack;  //���� ����������� � ������ ������
	else if (D && !H && I)		state = stateAttack;
	else if (D && !H && !I)		state = statePanic;
	else if (E && H && I)		state = stateAttack; 
	else if (E && H && !I)  	state = stateAttack;  //���� ����������� � ������ ������
	else if (E && !H && I) 		state = stateAttack;
	else if (E && !H && !I)		state = stateAttack;
	else if (F && H && I) 		state = stateAttack; 		
	else if (F && H && !I)  	state = stateAttack; 
	else if (A && !K && !H)		state = stateExploreNDE;  //SetState(stateExploreDNE);  // ����� ������� ����, �� �� ����, ���� �� ��������		(ExploreDNE)
	else if (A && !K && H)		state = stateExploreNDE;  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// ����� ������� ����, �� �� ����, ���� ��������			(ExploreDE)		
	else if (B && !K && !H)		state = stateExploreNDE;	// ����� �� ������� ����, �� �� ����, ���� �� ��������	(ExploreNDNE)
	else if (B && !K && H)		state = stateExploreNDE;	// ����� �� ������� ����, �� �� ����, ���� ��������		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))	
								state = stateEat;
	else						state = stateRest; 

	
	if (state == stateAttack) {
		look_at_enemy = true;
		// calc new target delta
		float yaw, pitch;
		Fvector().sub(m_tEnemy.obj->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;
		yaw = angle_normalize(yaw);

		if (from_right(yaw,m_body.current.yaw)) {
			_target_delta = angle_difference(yaw,m_body.current.yaw);
		} else _target_delta = -angle_difference(yaw,m_body.current.yaw);

		clamp(_target_delta, -PI_DIV_4, PI_DIV_4);
	}
	
	SetState(state);
}

void __stdcall CAI_Boar::BoneCallback(CBoneInstance *B)
{
	CAI_Boar	*P = dynamic_cast<CAI_Boar*> (static_cast<CObject*>(B->Callback_Param));

	if (!P->look_at_enemy) return;
	
	Fmatrix M;
	M.setXYZi (P->_cur_delta,0.0f, 0.0f);
	B->mTransform.mulB(M);
}

BOOL CAI_Boar::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head"));
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

	CMovementManager::m_body.target.yaw = angle_normalize(-yaw);
}


void CAI_Boar::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_ROTATION_JUMP) == ASP_ROTATION_JUMP) {
		float yaw, pitch;
		Fvector().sub(m_tEnemy.obj->Position(), Position()).getHP(yaw,pitch);
		yaw *= -1;
		yaw = angle_normalize(yaw);

		EMotionAnim anim = eAnimJumpLeft;
		if (from_right(yaw,m_body.current.yaw)) {
			anim = eAnimJumpRight;
			yaw = angle_normalize(yaw + PI / 20);	
		} else yaw = angle_normalize(yaw - PI / 20);

		MotionMan.Seq_Add(anim);
		MotionMan.Seq_Switch();

		CMovementManager::m_body.target.yaw = yaw;

		// calculate angular speed
		float new_angular_velocity; 
		float delta_yaw = angle_difference(yaw,m_body.current.yaw);
		float time = MotionMan.GetAnimTime(anim, 0);
		new_angular_velocity = 2.5f * delta_yaw / time; 

		MotionMan.ForceAngularSpeed(new_angular_velocity);

		Msg("!!!new angular = [%f]", new_angular_velocity);
	}
}

void CAI_Boar::UpdateCL()
{
	inherited::UpdateCL();
	angle_lerp(_cur_delta, _target_delta, _velocity, Device.fTimeDelta);
}


void CAI_Boar::ProcessTurn()
{
	float delta_yaw = angle_difference(m_body.target.yaw, m_body.current.yaw);
	if (delta_yaw < deg(1)) {
		m_body.current.yaw = m_body.target.yaw;
		return;
	}
	
	EMotionAnim anim = MotionMan.GetCurAnim();
	
	bool turn_left = true;
	if (from_right(m_body.target.yaw, m_body.current.yaw)) turn_left = false; 
 
	switch (anim) {
		case eAnimStandIdle: 
			(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			return;
		case eAnimJumpLeft:
		case eAnimJumpRight:
			MotionMan.SetCurAnim(anim);
			return;
		default:
			if (delta_yaw > deg(30)) {
				(turn_left) ? MotionMan.SetCurAnim(eAnimStandTurnLeft) : MotionMan.SetCurAnim(eAnimStandTurnRight);
			}
			return;
	}

}
