#include "stdafx.h"
#include "ai_flesh.h"
#include "../../ai_space.h"
#include "../ai_monster_utils.h"

CAI_Flesh::CAI_Flesh()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>	(this);
	stateExploreDE		= xr_new<CBitingExploreDE>	(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	CurrentState		= stateRest;

	stateSearchEnemy	= xr_new<CBitingSearchEnemy>(this);

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

	xr_delete(stateSearchEnemy);
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

	return TRUE;
}

void CAI_Flesh::Load(LPCSTR section)
{
	inherited::Load(section);

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::_sd->m_fsVelocityWalkFwdDamaged,	PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::_sd->m_fsVelocityRunFwdDamaged,		PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimAttackRat,		"stand_attack_rat_",	-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimAttackFromBack,	"stand_attack_back_",	-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_eat_",			 1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimDie,				"stand_die_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE,		"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimSteal,			"stand_crawl_",			-1, &inherited::_sd->m_fsVelocitySteal,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &inherited::_sd->m_fsVelocityDrag,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");

	MotionMan.AddAnim(eAnimScared,			"stand_scared_",		-1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_stand_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	
	// define transitions
	MotionMan.AddTransition(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	MotionMan.AddTransition(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/10);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack, eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_2);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimScared);
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

	MotionMan.AA_PushAttackAnimTest(eAnimAttack,		0, 400,		600,	STANDART_ATTACK,	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttack,		1, 600,		800,	STANDART_ATTACK,	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,3.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttackFromBack,0, 400,		600,	-PI_DIV_6 + PI, PI_DIV_6 + PI, -PI_DIV_6,PI_DIV_6, 3.5f,	inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,-3.f));
	MotionMan.AA_PushAttackAnimTest(eAnimAttackRat,		0, 600,		800,	-PI_DIV_6, PI_DIV_6, -PI_DIV_3 - PI_DIV_2, PI_DIV_3 - PI_DIV_2, 2.f, inherited::_sd->m_fHitPower, Fvector().set(0.f,0.f,1.f), AA_FLAG_ATTACK_RAT);

	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

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
	else if (D && !H && !I) 	SetState(statePanic);
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);		//тихо подобраться и начать аттаку
	else if (E && !H && I) 		SetState(stateAttack); 
	else if (E && !H && !I)		SetState(stateAttack); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateAttack); 
	else if (F && !H && !I) 	SetState(stateAttack);
	else if (A && !K)			SetState(statePanic);		//SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K)			SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	
#ifdef TEST_EAT_STATE	
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1))
		SetState(stateEat);
#else
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))	
		SetState(stateEat);
#endif
	else						SetState(stateRest); 

///	if ((CurrentState == stateAttack) && m_tEnemy.obj && (m_tEnemy.time + 1500 < m_current_update) ) 
///		SetState(stateSearchEnemy);


}

bool CAI_Flesh::AA_CheckHit()
{
	// Проверка состояния анимации (атака)
	SAttackAnimation apt_anim;

	bool was_hit = false;	

	if (MotionMan.AA_CheckTime(m_dwCurrentTime,apt_anim)) {
		CSoundPlayer::play(MonsterSpace::eMonsterSoundAttackHit);

		VisionElem ve;
		if (!GetEnemy(ve)) return false;
		const CObject *obj = dynamic_cast<const CObject *>(ve.obj);
	
		// перевод из локальных координат в мировые
		Fvector trace_from;
		XFORM().transform_tiny(trace_from, apt_anim.trace_from);
		
		Fvector trace_to;
		XFORM().transform_tiny(trace_to, apt_anim.trace_to);

		// вычисление вектора направления проверки хита
		Fvector dir;
		dir.sub(trace_to, trace_from);
		float dist = dir.magnitude();
		dir.normalize_safe();
		
		// делаем поправку на pitch (fix it)
		Fvector new_dir;
		Fvector C1, C2;
		Center(C1);
		obj->Center(C2);
		new_dir.sub(C2,C1);
		new_dir.normalize();
		dir.y = new_dir.y;
		dir.normalize_safe();
		
		// перевод из локальных координат в мировые вектора направления импульса
		Fvector hit_dir;
		XFORM().transform_dir(hit_dir,apt_anim.hit_dir);
		hit_dir.normalize();

#ifndef SIMPLE_ENEMY_HIT_TEST

		// трассировка нужна?
		if ((apt_anim.flags & AA_FLAG_FIRE_ANYWAY) == AA_FLAG_FIRE_ANYWAY) {
			HitEntity(ve.obj, apt_anim.damage,hit_dir);	// не нужна
			was_hit = true;
		} else if ((apt_anim.flags & AA_FLAG_ATTACK_RAT) == AA_FLAG_ATTACK_RAT) {

			// TestIntersection конуса(копыта) и сферы(крысы)
			Fvector dir;	dir.set(0.f,-1.f,0.f);		// направление конуса
			Fvector vC;		ve.obj->Center(vC);			// центр сферы

			if (ConeSphereIntersection(trace_from, PI_DIV_6, dir, vC, ve.obj->Radius())) {
				HitEntity(ve.obj,apt_anim.damage,dir);
				was_hit = true;
			}

		} else 	{ // нужна
			if (RayPickEnemy(obj, trace_from, dir, dist, 0.2f, 20)) {
				HitEntity(ve.obj, apt_anim.damage, hit_dir);
				was_hit = true;
			}
		}

#else
		dist = 20.f;

		bool should_hit = true;
		Fvector d;
		d.sub(C2,C1);
		if (d.magnitude() > apt_anim.dist) should_hit = false;

		float my_h,my_p;
		float h,p;

		Direction().getHP(my_h,my_p);
		d.getHP(h,p);

		float from	= angle_normalize(my_h + apt_anim.yaw_from);
		float to	= angle_normalize(my_h + apt_anim.yaw_to);

		if (!is_angle_between(h, from, to)) should_hit = false;

		from	= angle_normalize(my_p + apt_anim.pitch_from);
		to		= angle_normalize(my_p + apt_anim.pitch_to);

		if (!is_angle_between(p, from, to)) should_hit = false;

		if (should_hit) {
			HitEntity(ve.obj, apt_anim.damage, hit_dir);
			was_hit = true;
		}

#endif
		if (!ve.obj->g_Alive()) AddCorpse(ve);	
		
		if (AS_Active()) AS_Check(was_hit);
		MotionMan.AA_UpdateLastAttack(m_dwCurrentTime);
	}
	return was_hit;
}


// возвращает true, если после выполнения этой функции необходимо прервать обработку
// т.е. если активирована последовательность
void CAI_Flesh::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_DRAG_CORPSE) == 	ASP_DRAG_CORPSE) MotionMan.SetCurAnim(eAnimDragCorpse);

	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_BACK_ATTACK) == ASP_BACK_ATTACK) {
		MotionMan.Seq_Add(eAnimAttackFromBack);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_ATTACK_RAT) == ASP_ATTACK_RAT) MotionMan.SetCurAnim(eAnimAttackRat);
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) MotionMan.SetCurAnim(eAnimThreaten);
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

#ifdef DEBUG
void CAI_Flesh::OnRender()
{
	inherited::OnRender();
	
}
#endif

void CAI_Flesh::ProcessTurn()
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

