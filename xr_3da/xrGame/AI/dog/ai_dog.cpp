#include "stdafx.h"
#include "ai_dog.h"


CAI_Dog::CAI_Dog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	CurrentState		= stateRest;

	Init();
}

CAI_Dog::~CAI_Dog()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
}


void CAI_Dog::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}


void CAI_Dog::StateSelector()
{	
	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && H && !I)		SetState(stateAttack);  //тихо подобраться и начать аттаку
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(stateHide);	// отход перебежками через укрытия
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);  //тихо подобраться и начать аттаку
	else if (E && !H && I) 		SetState(stateDetour); 
	else if (E && !H && !I)		SetState(stateDetour); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateDetour); 
	else if (F && !H && !I) 	SetState(stateHide);
	else if (A && !K && !H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);  // слышу опасный звук, но не вижу, враг не выгодный		(ExploreDNE)
	else if (A && !K && H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// слышу опасный звук, но не вижу, враг выгодный			(ExploreDE)		
	else if (B && !K && !H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг не выгодный	(ExploreNDNE)
	else if (B && !K && H)		SetState(stateExploreNDE);	// слышу не опасный звук, но не вижу, враг выгодный		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 
}

//
//void CAI_Dog::LoadAttackAnim()
//{
//	Fvector center;
//
//	center.set		(0.f,0.f,0.f);
//
//	// 1 //
//	m_tAttackAnim.PushAttackAnim(0, 10, 0, 700,	800,	center,		2.f, m_fHitPower, 0.f, 0.f);
//
//	// 2 //
//	m_tAttackAnim.PushAttackAnim(0, 10, 1, 600,	800,	center,		2.5f, m_fHitPower, 0.f, 0.f);
//
//	// 3 // 
//	m_tAttackAnim.PushAttackAnim(0, 10, 2, 600, 700,	center,		1.5f, m_fHitPower, 0.f, 0.f);
//}


BOOL CAI_Dog::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0, 0, PS_STAND);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_RUN,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_EAT,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimStandIdle);
	MotionMan.LinkAction(ACT_STEAL,			eAnimStandIdle);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	return TRUE;
}

