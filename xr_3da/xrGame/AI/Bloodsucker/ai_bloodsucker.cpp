#include "stdafx.h"
#include "ai_bloodsucker.h"

CAI_Bloodsucker::CAI_Bloodsucker()
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

	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
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


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}


void CAI_Bloodsucker::Think()
{

	inherited::Think();

	// A - я слышу опасный звук
	// B - я слышу неопасный звук
	// С - я вижу очень опасного врага
	// D - я вижу опасного врага
	// E - я вижу равного врага
	// F - я вижу слабого врага
	// H - враг выгодный
	// I - враг видит меня
	// J - A | B
	// K - C | D | E | F 

	VisionElem ve;

	if (Motion.m_tSeq.Active())	{
		Motion.m_tSeq.Cycle(m_dwCurrentUpdate);
	}else {
		//- FSM 1-level 

		//if (flagEnemyLostSight && H && (E || F) && !A) SetState(stateFindEnemy);	// поиск врага
		if (C && H && I)		SetState(statePanic);
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
		else if (GetCorpse(ve) && ve.obj->m_fFood > 1)	
			SetState(stateEat);
		else						SetState(stateRest); 
		//-
		
		CurrentState->Execute(m_dwCurrentUpdate);

		// проверяем на завершённость
		if (CurrentState->CheckCompletion()) SetState(stateRest, true);
	}

	Motion.SetFrameParams(this);
	ControlAnimation();		
}

void CAI_Bloodsucker::UpdateCL()
{
	inherited::UpdateCL();
}


void CAI_Bloodsucker::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;	 index3 = -1;	break;
		case eMotionLieIdle:		index1 = 1; index2 = 0;	 index3 = -1;	break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 1;	 index3 = -1;	break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;	 index3 = -1;	break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 3;  index3 = -1;	break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 2;  index3 = -1;	break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 2;  index3 = -1;	break;
		case eMotionRun:			index1 = 0; index2 = 6;  index3 = -1;	break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 6;  index3 = -1;	break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 6;  index3 = -1;	break;
		case eMotionAttack:			index1 = 0; index2 = 9;  index3 = -1;	break;
		case eMotionAttackRat:		index1 = 0; index2 = 9; index3 = -1;	break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 6; index3 = -1;	break;
		case eMotionEat:			index1 = 1; index2 = 12; index3 = -1;	break;
		case eMotionStandDamaged:	index1 = 0; index2 = 0; index3 = -1;	break;
		case eMotionScared:			index1 = 0; index2 = 0; index3 = -1;	break;
		case eMotionDie:			index1 = 0; index2 = 0; index3 = -1;	break;
		case eMotionLieDown:		index1 = 0; index2 = 16; index3 = -1;	break;
		case eMotionStandUp:		index1 = 1; index2 = 17; index3 = -1;	break;
		case eMotionCheckCorpse:	index1 = 0; index2 = 0;	 index3 = -1;	break;
		case eMotionLieDownEat:		index1 = 0; index2 = 16; index3 = -1;	break;
		case eMotionAttackJump:		index1 = 0; index2 = 0; index3 = -1;	break;
			//default:					NODEFAULT;
	}
}

