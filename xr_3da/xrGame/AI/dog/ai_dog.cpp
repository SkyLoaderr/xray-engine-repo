#include "stdafx.h"
#include "ai_dog.h"


CAI_Dog::CAI_Dog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this);
	stateEat			= xr_new<CBitingEat>		(this);
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

//void CAI_Dog::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
//{
//	index1 = index2 = 0;		// bug protection ;) todo: find out the reason
//	index3 = -1;
//
//	switch(motion) {
//		case eAnimStandIdle:		index1 = 0; index2 = 0;	 break;
//		case eAnimLieIdle:			index1 = 2; index2 = 0;	 break;
//		case eAnimStandTurnLeft:	index1 = 0; index2 = 3;	 break;
//		case eAnimWalkFwd:			index1 = 0; index2 = 3;	 break;
//		case eAnimWalkBkwd:			index1 = 0; index2 = 3;  break;
//		case eAnimWalkTurnLeft:		index1 = 0; index2 = 3;  break;
//		case eAnimWalkTurnRight:	index1 = 0; index2 = 3;  break;
//		case eAnimRun:				index1 = 0; index2 = 7;  break;
//		case eAnimRunTurnLeft:		index1 = 0; index2 = 7;  break;
//		case eAnimRunTurnRight:		index1 = 0; index2 = 7;  break;
//		case eAnimAttack:			index1 = 0; index2 = 10;  break;
//		case eAnimAttackRat:		index1 = 0; index2 = 10; break;
//		case eAnimFastTurn:			index1 = 0; index2 = 7; break;
//		case eAnimEat:				index1 = 0; index2 = 14; break;
//		case eAnimStandDamaged:		index1 = 0; index2 = 15; break;
//		case eAnimScared:			index1 = 0; index2 = 0; break;
//		case eAnimDie:				index1 = 0; index2 = 0; break;
//		case eAnimStandLieDown:		index1 = 0; index2 = 20; break;
//		case eAnimLieStandUp:		index1 = 2; index2 = 21; break;
//		case eAnimCheckCorpse:		index1 = 0; index2 = 0;	 break;
//		case eAnimStandLieDownEat:	index1 = 0; index2 = 0; break;
//		case eAnimAttackJump:		index1 = 0; index2 = 10; break;
//			//default:					NODEFAULT;
//	}
//
////	if (index3 == -1) index3 = ::Random.randI((int)m_tAnimations.A[index1].A[index2].A.size());
//}
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

