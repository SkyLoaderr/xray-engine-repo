#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_effector.h"
#include "../ai_monsters_misc.h"

#include "..\\..\\hudmanager.h"

//#define AI_SQUAD_ENABLE


CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBitingRest>			(this);
	stateEat			= xr_new<CBitingEat>			(this, false);
	stateAttack			= xr_new<CBitingAttack>			(this, true);
	statePanic			= xr_new<CBitingPanic>			(this, false);
	stateExploreDNE		= xr_new<CBitingExploreDNE>		(this, true);
	stateExploreNDE		= xr_new<CBitingExploreNDE>		(this);
	stateSquadTask		= xr_new<CBitingSquadTask>		(this);
	
	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
	xr_delete(stateEat);
	xr_delete(stateAttack);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreNDE);
	xr_delete(stateSquadTask);
}


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	m_fEffectDist					= 0.f;

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Reset();

	last_time_finished				= 0;

	task							= 0;
}

void CAI_Bloodsucker::Load(LPCSTR section) 
{
	inherited::Load(section);

	CMonsterInvisibility::Load(section);

	m_fInvisibilityDist = pSettings->r_float(section,"InvisibilityDist");
	m_ftrPowerDown		= pSettings->r_float(section,"PowerDownFactor");	
	m_fPowerThreshold	= pSettings->r_float(section,"PowerThreshold");	
	m_fEffectDist		= pSettings->r_float(section,"EffectDistance");	

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);
	
	BEGIN_LOAD_SHARED_MOTION_DATA();

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, 0,										inherited::_sd->m_fsTurnNormalAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, 0,										inherited::_sd->m_fsTurnNormalAngular,	PS_STAND);
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, 0,										0,										PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, inherited::_sd->m_fsWalkFwdNormal,		inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, inherited::_sd->m_fsWalkFwdDamaged,		inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimWalkBkwd,		"stand_walk_bkwd_",		-1, inherited::_sd->m_fsWalkBkwdNormal,		inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	inherited::_sd->m_fsRunFwdNormal,		inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	inherited::_sd->m_fsRunFwdDamaged,		inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, 0,										inherited::_sd->m_fsRunAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimSitToSleep,		"sit_sleep_down_",		-1, 0,										0,										PS_SIT);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, inherited::_sd->m_fsSteal,				inherited::_sd->m_fsWalkAngular,		PS_STAND);
	MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, 0,										0,										PS_STAND);
	MotionMan.AddAnim(eAnimMiscAction_00,	"stand_to_aggressive_",	-1, 0,										0,										PS_STAND);	
	
	// define transitions
//	MotionMan.AddTransition(PS_STAND,			eAnimThreaten,	eAnimMiscAction_00,	false);
	MotionMan.AddTransition(eAnimStandSitDown,	eAnimSleep,		eAnimSitToSleep,	false);
	MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandSitDown,	true);
	MotionMan.AddTransition(PS_STAND,			PS_SIT,			eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_SIT,				PS_STAND,		eAnimSitStandUp,	false);
	MotionMan.AddTransition(PS_LIE,				PS_STAND,		eAnimSitStandUp,	false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/5);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack,	eAnimRun,			eAnimRun,			PI_DIV_6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround); 


	Fvector center;
	center.set		(0.f,0.f,0.f);

	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 500,	600,	center,		2.2f, inherited::_sd->m_fHitPower, -PI_DIV_6,	PI_DIV_6);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 600,	700,	center,		2.2f, inherited::_sd->m_fHitPower, 0.f,			PI_DIV_6);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 500,	600,	center,		2.3f, inherited::_sd->m_fHitPower, PI_DIV_3,	PI_DIV_6);

	//MotionMan.FX_LoadMap(section); 

	END_LOAD_SHARED_MOTION_DATA();
}


void __stdcall CAI_Bloodsucker::BoneCallback(CBoneInstance *B)
{
	CAI_Bloodsucker*	this_class = dynamic_cast<CAI_Bloodsucker*> (static_cast<CObject*>(B->Callback_Param));

	this_class->Bones.Update(B, Level().timeServer());
}


void CAI_Bloodsucker::vfAssignBones()
{
	// Установка callback на кости
	bone_spine =	&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head"));
	bone_spine->set_callback(BoneCallback,this);
	bone_head->set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_X);	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_head, AXIS_X);	Bones.AddBone(bone_head, AXIS_Y);
}


#define MAX_BONE_ANGLE PI_DIV_4

void CAI_Bloodsucker::LookDirection(Fvector to_dir, float bone_turn_speed)
{
	// получаем вектор направления к источнику звука и его мировые углы
	float		yaw,pitch;
	to_dir.getHP(yaw,pitch);

	// установить параметры вращения по yaw
	float cur_yaw = -m_body.current.yaw;						// текущий мировой угол монстра
	float bone_angle;											// угол для боны	

	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// дельта, на которую нужно поворачиваться

	if (angle_difference(cur_yaw,yaw) <= MAX_BONE_ANGLE) {		// bone turn only
		bone_angle = dy;
	} else {													// torso & bone turn 
		if (IsMoveAlongPathFinished() || !CMovementManager::enabled()) m_body.target.yaw = angle_normalize(-yaw);
		if (dy / 2 < MAX_BONE_ANGLE) bone_angle = dy / 2;
		else bone_angle = MAX_BONE_ANGLE;
	}

	bone_angle /= 2;
	if (IsRightSide(yaw,cur_yaw)) bone_angle *= -1.f;

	Bones.SetMotion(bone_spine, AXIS_X, bone_angle, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_X, bone_angle, bone_turn_speed, 100);

	// установить параметры вращения по pitch
	clamp(pitch, -MAX_BONE_ANGLE, MAX_BONE_ANGLE);
	pitch /= 2; 

	Bones.SetMotion(bone_spine, AXIS_Y, pitch, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_Y, pitch, bone_turn_speed, 100);	

	m_head = m_body;
}

void CAI_Bloodsucker::LookPosition(Fvector to_point, float angular_speed)
{
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());
	LookDirection(dir,angular_speed);
}

void CAI_Bloodsucker::ActivateEffector(float life_time)
{
	Level().Cameras.AddEffector(xr_new<CBloodsuckerEffector>(life_time));
}


void CAI_Bloodsucker::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
		return;
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		MotionMan.SetCurAnim(eAnimLookAround);
		return;
	}

}



BOOL CAI_Bloodsucker::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	return(TRUE);
}

void CAI_Bloodsucker::UpdateCL()
{
	inherited::UpdateCL();

	// Blink processing
	bool PrevVis	=	IsCurrentVisible();
	bool NewVis		=	CMonsterInvisibility::Update();
	if (NewVis != PrevVis) setVisible(NewVis);


	SquadDebug();

}

void CAI_Bloodsucker::StateSelector()
{
	VisionElem ve;

	// save CurrentState
	pState = CurrentState;

	if (C && H && I)			pState = statePanic; 
	else if (C && H && !I)		pState = statePanic;
	else if (C && !H && I)		pState = statePanic;
	else if (C && !H && !I) 	pState = statePanic;
	else if (D && H && I)		pState = stateAttack;
	else if (D && !H && I)		pState = statePanic;
	else if (D && !H && !I) 	pState = stateAttack;			// :: Hide
	else if (D && H && !I)		pState = stateAttack;
	else if (E && H && I)		pState = stateAttack;
	else if (E && H && !I)  	pState = stateAttack;
	else if (E && !H && I) 		pState = stateAttack;			// :: Detour
	else if (E && !H && !I)		pState = stateAttack;			// :: Detour 
	else if (F && H && I) 		pState = stateAttack; 		
	else if (F && H && !I)  	pState = stateAttack;
	else if (F && !H && I)  	pState = stateAttack;
	else if (F && !H && !I) 	pState = stateAttack;		
	else if (A && !K)			pState = stateExploreNDE; 
	else if (B && !K)			pState = stateExploreNDE; 
	else if ((GetCorpse(ve) && (ve.obj->m_fFood > 1)) && ((GetSatiety() < 0.85f) || flagEatNow))
		pState= stateEat;	
	else						pState = stateRest;

	EMotionAnim anim = MotionMan.Seq_CurAnim();
	if ((anim == eAnimCheckCorpse) && K) MotionMan.Seq_Finish();

#ifdef AI_SQUAD_ENABLE
	ProcessSquad();
#endif

	SetState(pState);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Process Squad AI


void CAI_Bloodsucker::DBG_TranslateTaskBefore()
{

	LOG_EX2("SQUAD:: [M =%s] Getting task [time = %u]: ", *"*/ cName(), m_dwCurrentTime /*"*);

	string32 s_type;

	switch (task->state.type) {
		case TS_REQUEST:	strcpy(s_type, "TS_REQUEST");	break;
		case TS_PROGRESS:	strcpy(s_type, "TS_PROGRESS");	break;
		case TS_REFUSED:	strcpy(s_type, "TS_REFUSED");	break;
	}	

	LOG_EX2("SQUAD:: [M =%s] Task desc: [type = %s], [ttl = %u]: ", *"*/ cName(), s_type, task->state.ttl /*"*);
}

void CAI_Bloodsucker::DBG_TranslateTaskAfter()
{

	LOG_EX2("SQUAD:: [M =%s] After: Task [time = %u]: ", *"*/ cName(), m_dwCurrentTime /*"*);

	string32 s_type;

	switch (task->state.type) {
		case TS_REQUEST:	strcpy(s_type, "TS_REQUEST");	break;
		case TS_PROGRESS:	strcpy(s_type, "TS_PROGRESS");	break;
		case TS_REFUSED:	strcpy(s_type, "TS_REFUSED");	break;
	}	

	LOG_EX2("SQUAD:: [M =%s] Task desc: [type = %s], [ttl = %u]: ", *"*/ cName(), s_type, task->state.ttl /*"*);
	LOG_EX2("SQUAD:: [M =%s] ------------------------------------ ", *"*/ cName() /*"*);
}


// ----------------------------------------------------------------------------------------------

void CAI_Bloodsucker::ProcessSquad()
{
	CMonsterSquad	*pSquad = Level().SquadMan.GetSquad((u8)g_Squad());

	// Если лидер - выполнить планирование (!!!)
	// Проверить условия перепланирования
	if ((pSquad->GetLeader() == this) && ShouldReplan()) pSquad->ProcessGroupIntel();
	
	// получить свою задачу
	task = &pSquad->GetTask(this);

	//DBG_TranslateTaskBefore();

	// Проверить на завершение задачи
	if (IsTaskActive() && IsTaskMustFinished()) StopTask();
	
	// Может быть запущена задача?
	if (CanExecuteSquadTask()) {
		
		bool bInitTask = !IsTaskActive();

		// Обновить состояние задачи
		UpdateTaskStatus();
		// Выполнить задачу
		ProcessTask(bInitTask);
	} else if (IsTaskActive()) StopTask();

	//DBG_TranslateTaskAfter();
}

// ----------------------------------------------------------------------------------------------

bool CAI_Bloodsucker::IsTaskActive()
{
	if (task->state.type == TS_PROGRESS) {
		if (task->state.ttl > m_dwCurrentTime) return true;
		else task->state.ttl = 0;
	}
		
	return false;
}

// ----------------------------------------------------------------------------------------------
bool CAI_Bloodsucker::IsTaskMustFinished()
{ 
	// сначала проверить задачу на завершение
	if (task->state.ttl < m_dwCurrentTime) return true;
	
	if (task->state.command == SC_FOLLOW) {
		if (task->target.pos.distance_to(Position()) < 1.0f) return true;
	}

	return false;
}
// ----------------------------------------------------------------------------------------------
void CAI_Bloodsucker::StopTask()
{
	task->state.ttl				= 0;
	last_time_finished			= m_dwCurrentTime;
	stateSquadTask->Done();
}
// ----------------------------------------------------------------------------------------------

bool CAI_Bloodsucker::CanExecuteSquadTask()
{
	return	(
		CheckValidity() &&				// проверить валидность задачи (по type && ttl)
		CheckCanSetWithTime() &&		// после выполненой задачи есть время, во время котторого новую брать задачу - нельзя
		SquadTaskIsHigherPriority() &&	// приоритет внутреннего состояния меньше приоритета задачи 
		CheckCanSetWithConditions()		// внутреннее состояние персонажа позволяют взять задание
		);
}

// ----------------------------------------------------------------------------------------------

bool CAI_Bloodsucker::CheckValidity()
{
	if (task->state.ttl == 0) return false;

	if (task->state.ttl < m_dwCurrentTime) {
		StopTask();
		return false;
	}

	if (task->state.type == TS_REQUEST) return true;
	if (task->state.type == TS_PROGRESS) return true;
	if (task->state.type == TS_REFUSED) return false; 

	return false;
}
// -----------------------------------------------------------------------------------------

#define TASK_MIN_INTERVAL		4000

bool CAI_Bloodsucker::CheckCanSetWithTime()
{
	if (IsTaskActive()) return true;

	if (last_time_finished + TASK_MIN_INTERVAL < m_dwCurrentTime) return true;
	return false;
}

// ----------------------------------------------------------------------------------------------

bool CAI_Bloodsucker::CheckCanSetWithConditions()
{
	return true;
}

// ----------------------------------------------------------------------------------------------

bool CAI_Bloodsucker::SquadTaskIsHigherPriority() 
{
	return (Level().SquadMan.TransformPriority(task->state.command) > pState->GetPriority());
}

// ----------------------------------------------------------------------------------------------

void CAI_Bloodsucker::UpdateTaskStatus()
{
	TTime ttl = 0;

	if (!IsTaskActive()) {// first time
		switch (task->state.command) {
		case SC_EXPLORE:		ttl = 10000;	break;
		case SC_ATTACK:			ttl = 5000;		break;
		case SC_THREATEN:		ttl = 3000;		break;
		case SC_COVER:			ttl = 10000;	break;
		case SC_FOLLOW:			ttl = 10000;	break;
		case SC_FEEL_DANGER:	ttl = 20000;	break;
		}

		task->state.type	= TS_PROGRESS;
		task->state.ttl		= m_dwCurrentTime + ttl;
	}

	task->state.type	= TS_PROGRESS;
}

// ----------------------------------------------------------------------------------------------

bool CAI_Bloodsucker::ShouldReplan()
{
	return true;
}

// ----------------------------------------------------------------------------------------------

void CAI_Bloodsucker::ProcessTask(bool bInit)
{
	pState = stateSquadTask;
}
 
// ----------------------------------------------------------------------------------------------

void CAI_Bloodsucker::SquadDebug()
{
	
	bool bThisIsLeader = Level().SquadMan.GetSquad((u8)g_Squad())->GetLeader() == this;

	if (bThisIsLeader) {
		HUD().pFontMedium->OutSet	(float(Device.dwWidth/2)-20.f,10.0f);
		HUD().pFontMedium->SetColor	(D3DCOLOR_XRGB(255,255,255));
		HUD().pFontMedium->OutNext	("TIME: %d", Level().timeServer());
	}

	Fmatrix res;
	res.mul(Device.mFullTransform,XFORM());

	Fvector3 v_src;
	Fvector4 v_res;
	v_src.set(0.f,2.2f,0.f);

	res.transform(v_res,v_src);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);
	
	// Show name
	HUD().pFontMedium->SetColor(0xff9999bb);
	HUD().pFontMedium->OutSet(x,y);
	
	if (bThisIsLeader) HUD().pFontMedium->OutNext("L:%s", cName());
	else HUD().pFontMedium->OutNext("%s", cName());

	// Show task
	HUD().pFontMedium->SetColor(0xffffff33);
	HUD().pFontMedium->OutSet(x,y-=20);
	
	if (!task) return;

	string32 s,s1,s2;
	if (!IsTaskActive()) strcpy(s,"T: NA");
	else {
		switch (task->state.type) {
			case TS_PROGRESS:	strcpy(s,"T:PRG"); break;
			case TS_REQUEST:	strcpy(s,"T:REQ");	break;
			case TS_REFUSED:	strcpy(s,"T:REF");	break;
		}

		if (task->state.type != TS_REFUSED) {

			switch (task->state.command) {
			case SC_EXPLORE:		strcpy(s2,"EXP");		break;
			case SC_ATTACK:			strcpy(s2,"ATT");		break;
			case SC_THREATEN:		strcpy(s2,"THR");		break;
			case SC_COVER:			strcpy(s2,"COV");		break;
			case SC_FOLLOW:			strcpy(s2,"FOL");		break;
			case SC_FEEL_DANGER:	strcpy(s2,"DNG");		break;
			}

			itoa(task->state.ttl,s1,10);
			strconcat(s,s,"|TTL:",s1,"|C:", s2);
		}
	}

	HUD().pFontMedium->OutNext("%s",s);
}
