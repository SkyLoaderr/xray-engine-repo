#include "stdafx.h"
#include "ai_flesh.h"

#include "..\\..\\ai_space.h"
using namespace AI;

CAI_Flesh::CAI_Flesh()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this);
	stateExploreDE		= xr_new<CBitingExploreDE>	(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	CurrentState		= stateRest;

	Init();
}

CAI_Flesh::~CAI_Flesh()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreDE);
	xr_delete(stateExploreNDE);
}

void CAI_Flesh::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
	m_fEyeShiftYaw					= PI_DIV_6;
}

BOOL CAI_Flesh::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_idle_ls_",		-1, 0,						m_ftrStandTurnRSpeed,		PS_STAND);
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_idle_ls_",		-1, 0,						m_ftrStandTurnRSpeed,		PS_STAND);
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, m_ftrWalkSpeed,			m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkBkwd,		"stand_walk_bkwd_",		-1, m_ftrWalkSpeed,			m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkTurnLeft,	"stand_walk_ls_",		-1, m_ftrWalkTurningSpeed,	m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimWalkTurnRight,	"stand_walk_rs_",		-1, m_ftrWalkTurningSpeed,	m_ftrWalkRSpeed,			PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	m_ftrRunAttackSpeed,	m_ftrRunRSpeed,				PS_STAND);
	MotionMan.AddAnim(eAnimRunTurnLeft,		"stand_run_ls_",		-1,	m_ftrRunAttackTurnSpeed,m_ftrRunAttackTurnRSpeed,	PS_STAND);
	MotionMan.AddAnim(eAnimRunTurnRight,	"stand_run_rs_",		-1,	m_ftrRunAttackTurnSpeed,m_ftrRunAttackTurnRSpeed,	PS_STAND);
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_idle_",			 2,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"lie_eat_",				-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimAttackRat,		"stand_attack2_",		 0, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		 0, 0,						m_ftrRunRSpeed,				PS_STAND);
	MotionMan.AddAnim(eAnimAttackJump,		"stand_attack_jump_",	 0,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimStandDamaged,	"stand_damaged_",		-1,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimStandLieDownEat, "stand_liedown_eat_",	-1,	0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_walk_bkwd_",		-1, m_ftrWalkSpeed/2,		m_ftrWalkRSpeed,			PS_STAND);

	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	MotionMan.AddTransition_S2A(PS_STAND,	eAnimEat,	eAnimStandLieDownEat,	false);
	MotionMan.AddTransition_S2S(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	MotionMan.AddTransition_S2S(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd,	eAnimWalkTurnLeft,	eAnimWalkTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun,		eAnimRunTurnLeft,	eAnimRunTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimRunTurnLeft, eAnimRunTurnRight, PI_DIV_6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimWalkFwd);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	// Добавить анимации атак
	Fvector center, left_side, right_side, special_side;

	center.set		(0.f,0.f,0.f);
	left_side.set	(-0.1f,0.f,0.f);
	right_side.set	(0.1f,0.f,0.f);
	special_side.set(-0.5f,0.f,0.5f);

	MotionMan.AA_PushAttackAnim(eAnimAttack,		0, 700,		800,	center,			2.f,  m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack,		1, 600,		800,	center,			2.5f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack,		2, 1100,	1250,	right_side,		1.5f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack,		3, 1300,	1400,	left_side,		0.6f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttackRat,		0, 600,		800,	special_side,	2.6f, m_fHitPower, 0.f, 0.f, AA_FLAG_ATTACK_RAT);
	MotionMan.AA_PushAttackAnim(eAnimAttackJump,	0, 700,		850,	center,			2.6f, m_fHitPower, 0.f, 0.f, AA_FLAG_FIRE_ANYWAY);

	return TRUE;
}

void CAI_Flesh::StateSelector()
{
	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && H && !I)		SetState(stateAttack);		//тихо подобраться и начать аттаку
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(stateHide);		// отход перебежками через укрытия
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);		//тихо подобраться и начать аттаку
	else if (E && !H && I) 		SetState(stateDetour); 
	else if (E && !H && !I)		SetState(stateDetour); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateDetour); 
	else if (F && !H && !I) 	SetState(stateHide);
	else if (A && !K && !H)		SetState(stateExploreNDE);  // SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
	else if (A && !K && H)		SetState(stateExploreNDE);  // SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K && !H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
	else if (B && !K && H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 
}

void CAI_Flesh::CheckAttackHit()
{
	// Проверка состояния анимации (атака)
	TTime cur_time = Level().timeServer();
	SAttackAnimation apt_anim;

	bool bGoodTime = MotionMan.AA_CheckTime(cur_time,apt_anim);

	if (bGoodTime) {
		VisionElem ve;
		if (!GetEnemy(ve)) return;
		CObject *obj = dynamic_cast<CObject *>(ve.obj);

		Fvector trace_from;
		Center(trace_from);
		trace_from.add(apt_anim.trace_offset);

		// трассировка нужна?
		if ((apt_anim.flags & AA_FLAG_FIRE_ANYWAY) == AA_FLAG_FIRE_ANYWAY) {
			DoDamage(ve.obj, apt_anim.damage,apt_anim.dir_yaw, apt_anim.dir_pitch);	// не нужна
			MotionMan.AA_UpdateLastAttack(cur_time);
		} else if ((apt_anim.flags & AA_FLAG_ATTACK_RAT) == AA_FLAG_ATTACK_RAT) {

			// TestIntersection конуса(копыта) и сферы(крысы)
			Fvector dir;	dir.set(0.f,-1.f,0.f);		// направление конуса
			Fvector vC;		ve.obj->Center(vC);			// центр сферы

			if (ConeSphereIntersection(trace_from, PI_DIV_6, dir, vC, ve.obj->Radius())) {
				DoDamage(ve.obj,apt_anim.damage,apt_anim.dir_yaw, apt_anim.dir_pitch);
				MotionMan.AA_UpdateLastAttack(cur_time);
			}

		} else 	{ // нужна
			this->setEnabled(false);
			Collide::ray_query	l_rq;
			
			if (Level().ObjectSpace.RayPick(trace_from, Direction(), apt_anim.dist, l_rq)) {
				if ((l_rq.O == obj) && (l_rq.range < apt_anim.dist)) {
					DoDamage(ve.obj, apt_anim.damage,apt_anim.dir_yaw, apt_anim.dir_pitch);
					MotionMan.AA_UpdateLastAttack(cur_time);
				}
			}
			this->setEnabled(true);			

			if (!ve.obj->g_Alive()) AddCorpse(ve);	
		}
	}
}

// возвращает true, если после выполнения этой функции необходимо прервать обработку
// т.е. если активирована последовательность
void CAI_Flesh::CheckSpecParams(u32 spec_params)
{

	if ((spec_params & ASP_DRAG_CORPSE) == 	ASP_DRAG_CORPSE) MotionMan.SetCurAnim(eAnimDragCorpse);
	else if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) MotionMan.SetCurAnim(eAnimStandDamaged);

	if ((spec_params & ASP_ATTACK_RAT) == ASP_ATTACK_RAT) {
		MotionMan.Seq_Add(eAnimAttackRat);
		MotionMan.Seq_Switch();
	} 
	
	if ((spec_params & ASP_ATTACK_RAT_JUMP) == ASP_ATTACK_RAT_JUMP) {
		MotionMan.Seq_Add(eAnimAttackJump);
		MotionMan.Seq_Switch();
	} 

	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	EMotionAnim cur_anim = MotionMan.GetCurAnim();
	bool bDamaged = (GetHealth() < 0.5f);

	if (cur_anim == eAnimStandIdle && bDamaged) MotionMan.SetCurAnim(eAnimStandDamaged);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Функция ConeSphereIntersection
// Пересечение конуса (не ограниченного) со сферой
// Необходима для определения пересечения копыта плоти с баунд-сферой крысы
// Параметры: ConeVertex - вершина конуса, ConeAngle - угол конуса (между поверхностью и высотой)
// ConeDir - направление конуса, SphereCenter - центр сферы, SphereRadius - радиус сферы
bool CAI_Flesh::ConeSphereIntersection(Fvector ConeVertex, float ConeAngle, Fvector ConeDir, Fvector SphereCenter, float SphereRadius)
{
	float fInvSin = 1.0f/_sin(ConeAngle);
	float fCosSqr = _cos(ConeAngle)*_cos(ConeAngle);

	
	Fvector kCmV;	kCmV.sub(SphereCenter,ConeVertex);
	Fvector kD		= kCmV;
	Fvector tempV	= ConeDir;
	tempV.mul		(SphereRadius* fInvSin);
	kD.add			(tempV);

	float fDSqrLen = kD.square_magnitude();
	float fE = kD.dotproduct(ConeDir);
	if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr ) {
		
		float fSinSqr = _sin(ConeAngle)*_sin(ConeAngle);

		fDSqrLen = kCmV.square_magnitude();
		fE = -kCmV.dotproduct(ConeDir);
		if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr ) {
			float fRSqr = SphereRadius*SphereRadius;
			return fDSqrLen <= fRSqr;
		} else return true;
	} 
	
	return false;
}

